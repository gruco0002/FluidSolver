#pragma once

#include "FluidInclude.hpp"
#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace FluidSolver
{

    struct IISPHSettings3D
    {
        pFloat max_density_error_allowed = 0.001f;

        size_t min_number_of_iterations = 2;
        size_t max_number_of_iterations = 100;

        pFloat omega = 0.5f;
        pFloat gamma = 0.7f;

        pFloat viscosity = 5.0f;
    };

    struct IISPHParticleData3D
    {
        vec3 predicted_velocity;
        pFloat source_term;
        pFloat diagonal_element;
        pFloat predicted_density_error;
    };


    template <typename Kernel = CubicSplineKernel3D, typename NeighborhoodSearch = QuadraticNeighborhoodSearch3D,
              typename parallel = StdParallelForEach>
    class IISPHFluidSolver3D : public IFluidSolverBase {

      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        IISPHSettings3D settings;

      private:
        float current_timestep = 0.0f;

        void adapt_collection()
        {
            FLUID_ASSERT(!collection->is_type_present<IISPHParticleData>());
            collection->add_type<IISPHParticleData3D>();
        }

        vec3 ComputeViscosityAcceleration(pIndex_t particleIndex)
        {
            auto& movement_data = collection->get<MovementData3D>(particleIndex);

            const vec3& position = movement_data.position;
            const vec3& velocity = movement_data.velocity;


            vec3 tmp = vec3(0.0f);
            auto neighbors = neighborhood_search.get_neighbors(particleIndex);
            for (uint32_t neighbor : neighbors)
            {
                auto type = collection->get<ParticleInfo>(neighbor).type;
                if (type == ParticleTypeDead)
                {
                    continue; // don*t calculate unnecessary values for dead particles.
                }

                const vec3& neighborPosition = collection->get<MovementData3D>(neighbor).position;
                const vec3& neighborVelocity = collection->get<MovementData3D>(neighbor).velocity;
                float neighborMass = collection->get<ParticleData>(neighbor).mass;
                float neighborDensity = collection->get<ParticleData>(neighbor).density;

                if (neighborDensity == 0.0f)
                    continue;

                vec3 vij = velocity - neighborVelocity;
                vec3 xij = position - neighborPosition;

                tmp += (neighborMass / neighborDensity) *
                       (glm::dot(vij, xij) /
                        (glm::dot(xij, xij) + 0.01f * parameters.particle_size * parameters.particle_size)) *
                       kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            }

            return 2.0f * settings.viscosity * tmp;
        }

      public:
        virtual void execute_simulation_step(pFloat timestep) override
        {
            FLUID_ASSERT(collection != nullptr)
            FLUID_ASSERT(collection->is_type_present<MovementData3D>());
            FLUID_ASSERT(collection->is_type_present<ParticleData>());
            FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
            FLUID_ASSERT(collection->is_type_present<ExternalForces3D>());
            FLUID_ASSERT(collection->is_type_present<IISPHParticleData3D>());

            FLUID_ASSERT(settings.max_number_of_iterations >= settings.min_number_of_iterations);
            FLUID_ASSERT(settings.max_density_error_allowed > 0.0f);

            FLUID_ASSERT(timestep > 0.0f);

            // set the current timestep
            current_timestep = timestep;

            // find neighbors for all particles
            FLUID_ASSERT(neighborhood_search.collection == collection);
            neighborhood_search.find_neighbors();

            // set pressure to zero, calculate density, calculate non pressure accelerations and predicted velocity
            parallel::loop_for(0, collection->size(), [&](size_t particle_index) {
                auto particle_type = collection->get<ParticleInfo>(particle_index).type;

                if (particle_type == ParticleTypeDead)
                {
                    return; // don't calculate unnecessary data
                }

                auto& particle_data = collection->get<ParticleData>(particle_index);
                auto& movement_data = collection->get<MovementData3D>(particle_index);

                // set pressure to zero
                particle_data.pressure = 0.0f;

                // compute non pressure accelerations and predicted velocity
                {

                    glm::vec3& nonPressureAcc =
                        collection->get<ExternalForces3D>(particle_index).non_pressure_acceleration;
                    const glm::vec3& velocity = movement_data.velocity;

                    // adding gravity to non pressure acceleration
                    if (particle_type != ParticleTypeBoundary)
                    {
                        nonPressureAcc += glm::vec3(0.0f, -parameters.gravity, 0.0f);
                        nonPressureAcc += ComputeViscosityAcceleration(particle_index);
                    }

                    // calculate predicted velocity
                    glm::vec3 predictedVelocity = velocity + current_timestep * nonPressureAcc;

                    // set predicted velocity and reset non pressure accelerations
                    collection->get<IISPHParticleData3D>(particle_index).predicted_velocity = predictedVelocity;
                    nonPressureAcc = glm::vec3(0.0f);
                }

                // compute density
                {

                    if (particle_type == ParticleTypeBoundary)
                    {
                        return; // don't calculate density for the boundary particles
                    }

                    float density = 0.0f;

                    const glm::vec3& position = movement_data.position;
                    auto neighbors = neighborhood_search.get_neighbors(particle_index);
                    for (uint32_t neighbor : neighbors)
                    {
                        auto type = collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead)
                        {
                            continue; // don't calculate unnecessary values for dead particles.
                        }
                        const glm::vec3& neighbor_position = collection->get<MovementData3D>(neighbor).position;
                        float neighbor_mass = collection->get<ParticleData>(neighbor).mass;
                        density += neighbor_mass * kernel.GetKernelValue(neighbor_position, position);
                    }

                    particle_data.density = density;
                }
            });


            // compute source term and diagonal element
            parallel::loop_for(0, collection->size(), [&](size_t i) {
                auto type = collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeDead)
                    return; // we do not want to process dead particles
                if (type == ParticleTypeBoundary)
                    return; // we do not want to process boundary particles

                auto& iisph_data = collection->get<IISPHParticleData3D>(i);
                auto& movement_data = collection->get<MovementData3D>(i);
                auto& particle_data = collection->get<ParticleData>(i);

                // compute source term
                {
                    float neighbor_contribution = 0.0f;
                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors)
                    {
                        auto type = collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead)
                        {
                            continue; // don*t calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal)
                        {
                            neighbor_contribution +=
                                neighbor_particle_data.mass *
                                glm::dot(iisph_data.predicted_velocity - neighbor_iisph_data.predicted_velocity,
                                         kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                 movement_data.position));
                        }
                        else if (type == ParticleTypeBoundary)
                        {
                            neighbor_contribution +=
                                neighbor_particle_data.mass *
                                glm::dot(iisph_data.predicted_velocity,
                                         kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                 movement_data.position));
                        }
                    }

                    iisph_data.source_term =
                        parameters.rest_density - particle_data.density - current_timestep * neighbor_contribution;
                }

                // compute diagonal element
                {

                    // calculate the inner part of the sum
                    vec3 inner_part_of_sum = vec3(0.0f);
                    {
                        auto neighbors = neighborhood_search.get_neighbors(i);
                        for (auto neighbor : neighbors)
                        {
                            auto type = collection->get<ParticleInfo>(neighbor).type;
                            if (type == ParticleTypeDead)
                            {
                                continue; // don't calculate unnecessary values for dead particles.
                            }

                            auto& neighbor_particle_data = collection->get<ParticleData>(neighbor);
                            auto& neighbor_iisph_data = collection->get<IISPHParticleData3D>(neighbor);
                            auto& neighbor_movement_data = collection->get<MovementData3D>(neighbor);

                            if (type == ParticleTypeNormal)
                            {
                                inner_part_of_sum -= neighbor_particle_data.mass / Math::pow2(parameters.rest_density) *
                                                     kernel.GetKernelDerivativeReversedValue(
                                                         neighbor_movement_data.position, movement_data.position);
                            }
                            else if (type == ParticleTypeBoundary)
                            {
                                inner_part_of_sum -= 2.0f * settings.gamma * neighbor_particle_data.mass /
                                                     Math::pow2(parameters.rest_density) *
                                                     kernel.GetKernelDerivativeReversedValue(
                                                         neighbor_movement_data.position, movement_data.position);
                            }
                        }
                    }

                    // calculate the diagonal element
                    float diagonal_element = 0.0f;
                    {

                        auto neighbors = neighborhood_search.get_neighbors(i);
                        for (auto neighbor : neighbors)
                        {
                            auto type = collection->get<ParticleInfo>(neighbor).type;
                            if (type == ParticleTypeDead)
                            {
                                continue; // don't calculate unnecessary values for dead particles.
                            }

                            auto& neighbor_particle_data = collection->get<ParticleData>(neighbor);
                            auto& neighbor_iisph_data = collection->get<IISPHParticleData3D>(neighbor);
                            auto& neighbor_movement_data = collection->get<MovementData3D>(neighbor);

                            if (type == ParticleTypeNormal)
                            {
                                diagonal_element +=
                                    neighbor_particle_data.mass *
                                    glm::dot(inner_part_of_sum,
                                             kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                     movement_data.position));

                                diagonal_element +=
                                    neighbor_particle_data.mass *
                                    glm::dot(particle_data.mass / Math::pow2(parameters.rest_density) *
                                                 kernel.GetKernelDerivativeReversedValue(
                                                     movement_data.position, neighbor_movement_data.position),
                                             kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                     movement_data.position));
                            }
                            else if (type == ParticleTypeBoundary)
                            {
                                diagonal_element +=
                                    neighbor_particle_data.mass *
                                    glm::dot(inner_part_of_sum,
                                             kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                     movement_data.position));
                            }
                        }

                        diagonal_element *= Math::pow2(current_timestep);
                    }
                    iisph_data.diagonal_element = diagonal_element;
                }
            });

            // start iterations
            for (size_t iteration = 0; iteration < settings.max_number_of_iterations; iteration++)
            {

                // compute pressure acceleration
                parallel::loop_for(0, collection->size(), [&](size_t i) {
                    auto type = collection->get<ParticleInfo>(i).type;
                    if (type == ParticleTypeDead)
                        return; // we do not want to process dead particles
                    if (type == ParticleTypeBoundary)
                        return; // we do not want to process boundary particles

                    auto& iisph_data = collection->get<IISPHParticleData3D>(i);
                    auto& movement_data = collection->get<MovementData3D>(i);
                    auto& particle_data = collection->get<ParticleData>(i);

                    vec3 pressure_acceleration = vec3(0.0f);

                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors)
                    {
                        auto type = collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead)
                        {
                            continue; // don't calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal)
                        {
                            pressure_acceleration -=
                                neighbor_particle_data.mass *
                                (particle_data.pressure / Math::pow2(parameters.rest_density) +
                                 neighbor_particle_data.pressure / Math::pow2(parameters.rest_density)) *
                                kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                        movement_data.position);
                        }
                        else if (type == ParticleTypeBoundary)
                        {
                            pressure_acceleration -= settings.gamma * neighbor_particle_data.mass * 2.0f *
                                                     particle_data.pressure / Math::pow2(parameters.rest_density) *
                                                     kernel.GetKernelDerivativeReversedValue(
                                                         neighbor_movement_data.position, movement_data.position);
                        }
                    }

                    movement_data.acceleration = pressure_acceleration;
                });


                // compute divergence of the velocity change, update pressure, compute predicted density error per
                // particle
                parallel::loop_for(0, collection->size(), [&](size_t i) {
                    auto type = collection->get<ParticleInfo>(i).type;
                    if (type == ParticleTypeDead)
                        return; // we do not want to process dead particles
                    if (type == ParticleTypeBoundary)
                        return; // we do not want to process boundary particles

                    auto& iisph_data = collection->get<IISPHParticleData3D>(i);
                    auto& movement_data = collection->get<MovementData3D>(i);
                    auto& particle_data = collection->get<ParticleData>(i);

                    float ap = 0.0f;

                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors)
                    {
                        auto type = collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead)
                        {
                            continue; // don't calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal)
                        {
                            ap += neighbor_particle_data.mass *
                                  glm::dot(movement_data.acceleration - neighbor_movement_data.acceleration,
                                           kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                   movement_data.position));
                        }
                        else if (type == ParticleTypeBoundary)
                        {
                            ap += neighbor_particle_data.mass *
                                  glm::dot(movement_data.acceleration,
                                           kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                                                   movement_data.position));
                        }
                    }

                    ap *= Math::pow2(current_timestep);

                    if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon())
                    {
                        particle_data.pressure =
                            std::fmax(0.0f, particle_data.pressure + settings.omega * ((iisph_data.source_term - ap) /
                                                                                       iisph_data.diagonal_element));
                    }
                    else
                    {
                        // particle has no neighbors, hence set pressure to zero
                        particle_data.pressure = 0.0f;
                    }


                    // Info: The calculation of the particle density error is based on the implementation of IISPH by
                    // Stefan Band
                    float particle_density_error = std::abs(iisph_data.source_term - ap);
                    if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon())
                    {
                        if (std::abs(particle_data.pressure) <= std::numeric_limits<float>::epsilon())
                        {
                            // if the new pressure is zero, we do not have a density error
                            particle_density_error = 0.0f;
                        }

                        iisph_data.predicted_density_error = particle_density_error;
                    }
                });

                // check if we need further iterations
                {
                    float max_predicted_density_error = 0.0f;
                    float average_predicted_density_error = 0.0f;
                    size_t average_counter = 0;
                    for (size_t i = 0; i < collection->size(); i++)
                    {
                        auto type = collection->get<ParticleInfo>(i).type;
                        if (type != ParticleTypeNormal)
                        {
                            continue;
                        }
                        auto& iisph_data = collection->get<IISPHParticleData3D>(i);
                        if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon())
                        {
                            // this particle contributes to the density error
                            max_predicted_density_error =
                                std::fmax(max_predicted_density_error, iisph_data.predicted_density_error);
                            average_predicted_density_error += iisph_data.predicted_density_error;
                            average_counter++;
                        }
                    }

                    if (average_counter > 0)
                    {
                        average_predicted_density_error = average_predicted_density_error / (float)average_counter;
                    }

                    if (iteration >= settings.min_number_of_iterations - 1)
                    {
                        // we have at least reached the minimum number of iterations
                        if (average_predicted_density_error <= settings.max_density_error_allowed)
                        {
                            // exit the solver, since the average predicted density error has become small enough
                            break;
                        }
                    }
                }
            }

            // integrate particle movement
            parallel::loop_for(0, collection->size(), [&](size_t i) {
                // update velocity and position of all particles

                auto type = collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeBoundary)
                {
                    return; // don't calculate unnecessary values for the boundary particles.
                }
                if (type == ParticleTypeDead)
                {
                    return; // don't calculate unnecessary values for dead particles.
                }

                auto& movement_data = collection->get<MovementData3D>(i);

                // integrate using euler cromer
                movement_data.velocity = collection->get<IISPHParticleData3D>(i).predicted_velocity +
                                         current_timestep * movement_data.acceleration;
                movement_data.position = movement_data.position + current_timestep * movement_data.velocity;
            });
        }

        virtual void initialize() override
        {
            FLUID_ASSERT(collection != nullptr);
            if (!collection->is_type_present<IISPHParticleData3D>())
                adapt_collection();

            neighborhood_search.collection = collection;
            neighborhood_search.search_radius = parameters.particle_size * 2.0f;
            neighborhood_search.initialize();
            kernel.kernel_support = parameters.particle_size * 2.0f;
            kernel.initialize();
        }

        virtual NeighborhoodInterface create_neighborhood_interface() override
        {
            return neighborhood_search.create_interface();
        }

        virtual Compatibility check() override
        {
            Compatibility c;
            if (collection == nullptr)
            {
                c.add_issue({"IISPHFluidSolver3D", "ParticleCollection is null."});
            }
            else
            {
                if (!collection->is_type_present<MovementData3D>())
                {
                    c.add_issue({"IISPHFluidSolver3D", "Particles are missing the MovementData3D attribute."});
                }
                if (!collection->is_type_present<ParticleData>())
                {
                    c.add_issue({"IISPHFluidSolver3D", "Particles are missing the ParticleData attribute."});
                }
                if (!collection->is_type_present<ParticleInfo>())
                {
                    c.add_issue({"IISPHFluidSolver3D", "Particles are missing the ParticleInfo attribute."});
                }
                if (!collection->is_type_present<ExternalForces3D>())
                {
                    c.add_issue({"IISPHFluidSolver3D", "Particles are missing the ExternalForces3D attribute."});
                }
                if (!collection->is_type_present<IISPHParticleData3D>())
                {
                    c.add_issue({"IISPHFluidSolver3D", "Particles are missing the IISPHParticleData3D attribute."});
                }
            }

            if (settings.max_number_of_iterations < settings.min_number_of_iterations)
            {
                c.add_issue({"IISPHFluidSolver3D", "Max iterations are less than min number of iterations."});
            }

            if (settings.max_density_error_allowed <= 0.0f)
            {
                c.add_issue({"IISPHFluidSolver3D", "MaxDensityErrorAllowed is smaller or equal to zero."});
            }

            c.add_compatibility(neighborhood_search.check());
            c.add_compatibility(kernel.check());

            return c;
        }
    };

} // namespace FluidSolver