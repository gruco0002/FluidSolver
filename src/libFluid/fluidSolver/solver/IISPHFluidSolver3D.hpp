#pragma once

#include "FluidInclude.hpp"
#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace LibFluid {

    struct IISPHSettings3D : public DataChangeStruct {
        pFloat max_density_error_allowed = 0.001f;

        size_t min_number_of_iterations = 2;
        size_t max_number_of_iterations = 100;

        pFloat omega = 0.5f;

        // TODO: rename gamma to single_layer_boundary_gamma_2 if we can fully ignore gamma in multi layer scenarios
        pFloat gamma = 0.7f;

        pFloat viscosity = 5.0f;

        bool single_layer_boundary = false;
        float single_layer_boundary_gamma_1 = 1.1f;
    };

    struct IISPHParticleData3D {
        vec3 predicted_velocity;
        vec3 original_non_pressure_acceleration;
        pFloat source_term;
        pFloat diagonal_element;
        pFloat predicted_density_error;
    };


    template<typename Kernel = CubicSplineKernel3D, typename NeighborhoodSearch = QuadraticNeighborhoodSearch3D,
            typename parallel = StdParallelForEach>
    class IISPHFluidSolver3D : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        IISPHSettings3D settings;

        size_t stat_last_iteration_count = 0;
        float stat_last_average_predicted_density_error = 0.0f;

      private:
        float current_timestep = 0.0f;

        void adapt_collection() {
            FLUID_ASSERT(!data.collection->is_type_present<IISPHParticleData3D>());
            data.collection->add_type<IISPHParticleData3D>();
        }

        vec3 ComputeViscosityAcceleration(pIndex_t particleIndex) {
            auto& movement_data = data.collection->get<MovementData3D>(particleIndex);

            const vec3& position = movement_data.position;
            const vec3& velocity = movement_data.velocity;


            vec3 tmp = vec3(0.0f);
            auto neighbors = neighborhood_search.get_neighbors(particleIndex);
            for (uint32_t neighbor : neighbors) {
                auto type = data.collection->get<ParticleInfo>(neighbor).type;
                if (type == ParticleTypeDead) {
                    continue; // don*t calculate unnecessary values for dead particles.
                }

                const vec3& neighborPosition = data.collection->get<MovementData3D>(neighbor).position;
                const vec3& neighborVelocity = data.collection->get<MovementData3D>(neighbor).velocity;
                float neighborMass = data.collection->get<ParticleData>(neighbor).mass;
                float neighborDensity = data.collection->get<ParticleData>(neighbor).density;

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

        void initialize() override {
            FLUID_ASSERT(data.collection != nullptr);
            if (data.has_data_changed()) {
                data.acknowledge_data_change();

                if (!data.collection->is_type_present<IISPHParticleData3D>())
                    adapt_collection();

                neighborhood_search.collection = data.collection;
                neighborhood_search.initialize();
            }

            if (parameters.has_data_changed()) {
                parameters.acknowledge_data_change();

                neighborhood_search.search_radius = parameters.particle_size * 2.0f;
                neighborhood_search.initialize();
                kernel.kernel_support = parameters.particle_size * 2.0f;
                kernel.initialize();
            }

            if (settings.has_data_changed()) {
                settings.acknowledge_data_change();
            }
        }

        void execute_neighborhood_search() override {
            initialize();

            FLUID_ASSERT(data.collection != nullptr)
            FLUID_ASSERT(data.collection->is_type_present<MovementData3D>());
            FLUID_ASSERT(data.collection->is_type_present<ParticleData>());
            FLUID_ASSERT(data.collection->is_type_present<ParticleInfo>());
            FLUID_ASSERT(data.collection->is_type_present<ExternalForces3D>());
            FLUID_ASSERT(data.collection->is_type_present<IISPHParticleData3D>());

            // find neighbors for all particles
            FLUID_ASSERT(neighborhood_search.collection == data.collection);
            neighborhood_search.find_neighbors();
        }

        void execute_simulation_step(Timepoint& timestep) override {
            initialize();

            FLUID_ASSERT(data.collection != nullptr)
            FLUID_ASSERT(data.collection->is_type_present<MovementData3D>());
            FLUID_ASSERT(data.collection->is_type_present<ParticleData>());
            FLUID_ASSERT(data.collection->is_type_present<ParticleInfo>());
            FLUID_ASSERT(data.collection->is_type_present<ExternalForces3D>());
            FLUID_ASSERT(data.collection->is_type_present<IISPHParticleData3D>());

            FLUID_ASSERT(settings.max_number_of_iterations >= settings.min_number_of_iterations);
            FLUID_ASSERT(settings.max_density_error_allowed > 0.0f);

            FLUID_ASSERT(timestep.desired_time_step > 0.0f);

            FLUID_ASSERT(data.timestep_generator != nullptr);

            // set the current timestep
            current_timestep = timestep.desired_time_step;

            // set pressure to zero, calculate density, calculate non pressure accelerations and predicted velocity
            parallel::loop_for(0, data.collection->size(), [&](size_t particle_index) {
                auto particle_type = data.collection->get<ParticleInfo>(particle_index).type;

                if (particle_type == ParticleTypeDead) {
                    return; // don't calculate unnecessary data
                }

                auto& particle_data = data.collection->get<ParticleData>(particle_index);
                auto& movement_data = data.collection->get<MovementData3D>(particle_index);

                // set pressure to zero
                particle_data.pressure = 0.0f;

                // compute non pressure accelerations and predicted velocity
                {
                    glm::vec3& nonPressureAcc =
                            data.collection->get<ExternalForces3D>(particle_index).non_pressure_acceleration;
                    const glm::vec3& velocity = movement_data.velocity;

                    // adding gravity to non pressure acceleration
                    if (particle_type != ParticleTypeBoundary) {
                        nonPressureAcc += glm::vec3(0.0f, -parameters.gravity, 0.0f);
                        nonPressureAcc += ComputeViscosityAcceleration(particle_index);
                    }

                    // calculate predicted velocity
                    glm::vec3 predictedVelocity = velocity + current_timestep * nonPressureAcc;

                    // set predicted velocity and original non pressure acceleration
                    auto& ip = data.collection->get<IISPHParticleData3D>(particle_index);
                    ip.predicted_velocity = predictedVelocity;
                    ip.original_non_pressure_acceleration = nonPressureAcc;

                    // reset non pressure accelerations
                    nonPressureAcc = glm::vec3(0.0f);
                }

                // compute density
                {
                    if (particle_type == ParticleTypeBoundary) {
                        return; // don't calculate density for the boundary particles
                    }

                    float density = 0.0f;

                    const glm::vec3& position = movement_data.position;
                    auto neighbors = neighborhood_search.get_neighbors(particle_index);
                    for (uint32_t neighbor : neighbors) {
                        auto type = data.collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead) {
                            continue; // don't calculate unnecessary values for dead particles.
                        }

                        if (!settings.single_layer_boundary) {
                            // multi layer boundaries are expected
                            const glm::vec3& neighbor_position = data.collection->get<MovementData3D>(neighbor).position;
                            float neighbor_mass = data.collection->get<ParticleData>(neighbor).mass;
                            density += neighbor_mass * kernel.GetKernelValue(neighbor_position, position);
                        } else {
                            // single layer boundaries are activated
                            const glm::vec3& neighbor_position = data.collection->get<MovementData3D>(neighbor).position;
                            float neighbor_mass = data.collection->get<ParticleData>(neighbor).mass;
                            if (type == ParticleTypeBoundary) {
                                neighbor_mass *= settings.single_layer_boundary_gamma_1; // scale the boundary particles contribution
                            }
                            density += neighbor_mass * kernel.GetKernelValue(neighbor_position, position);
                        }
                    }

                    particle_data.density = density;
                }
            });


            // compute source term and diagonal element
            parallel::loop_for(0, data.collection->size(), [&](size_t i) {
                auto type = data.collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeDead)
                    return; // we do not want to process dead particles
                if (type == ParticleTypeBoundary)
                    return; // we do not want to process boundary particles

                auto& iisph_data = data.collection->get<IISPHParticleData3D>(i);
                auto& movement_data = data.collection->get<MovementData3D>(i);
                auto& particle_data = data.collection->get<ParticleData>(i);

                // compute source term
                {
                    float neighbor_contribution = 0.0f;
                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors) {
                        auto type = data.collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead) {
                            continue; // don*t calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = data.collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = data.collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = data.collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal) {
                            neighbor_contribution +=
                                    neighbor_particle_data.mass *
                                    glm::dot(iisph_data.predicted_velocity - neighbor_iisph_data.predicted_velocity,
                                            kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                    movement_data.position));
                        } else if (type == ParticleTypeBoundary) {
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
                        for (auto neighbor : neighbors) {
                            auto type = data.collection->get<ParticleInfo>(neighbor).type;
                            if (type == ParticleTypeDead) {
                                continue; // don't calculate unnecessary values for dead particles.
                            }

                            auto& neighbor_particle_data = data.collection->get<ParticleData>(neighbor);
                            auto& neighbor_iisph_data = data.collection->get<IISPHParticleData3D>(neighbor);
                            auto& neighbor_movement_data = data.collection->get<MovementData3D>(neighbor);

                            if (type == ParticleTypeNormal) {
                                inner_part_of_sum -= neighbor_particle_data.mass / Math::pow2(parameters.rest_density) *
                                        kernel.GetKernelDerivativeReversedValue(
                                                neighbor_movement_data.position, movement_data.position);
                            } else if (type == ParticleTypeBoundary) {
                                // TODO: gamma is used here even if single layer boundaries are deactivated?
                                if (settings.single_layer_boundary) {
                                    inner_part_of_sum -= 2.0f * settings.gamma * neighbor_particle_data.mass /
                                            Math::pow2(parameters.rest_density) *
                                            kernel.GetKernelDerivativeReversedValue(
                                                    neighbor_movement_data.position, movement_data.position);
                                } else {
                                    // TODO: check if we can leave out gamma here
                                    inner_part_of_sum -= 2.0f * neighbor_particle_data.mass /
                                            Math::pow2(parameters.rest_density) *
                                            kernel.GetKernelDerivativeReversedValue(
                                                    neighbor_movement_data.position, movement_data.position);
                                }
                            }
                        }
                    }

                    // calculate the diagonal element
                    float diagonal_element = 0.0f;
                    {
                        auto neighbors = neighborhood_search.get_neighbors(i);
                        for (auto neighbor : neighbors) {
                            auto type = data.collection->get<ParticleInfo>(neighbor).type;
                            if (type == ParticleTypeDead) {
                                continue; // don't calculate unnecessary values for dead particles.
                            }

                            auto& neighbor_particle_data = data.collection->get<ParticleData>(neighbor);
                            auto& neighbor_iisph_data = data.collection->get<IISPHParticleData3D>(neighbor);
                            auto& neighbor_movement_data = data.collection->get<MovementData3D>(neighbor);

                            if (type == ParticleTypeNormal) {
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
                            } else if (type == ParticleTypeBoundary) {
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

            // keep track of the final maximal velocity and acceleration of all fluid particles
            float max_final_velocity_squared = 0.0f;
            float max_final_acceleration_squared = 0.0f;

            // start iterations
            for (size_t iteration = 0; iteration < settings.max_number_of_iterations; iteration++) {
                // compute pressure acceleration
                parallel::loop_for(0, data.collection->size(), [&](size_t i) {
                    auto type = data.collection->get<ParticleInfo>(i).type;
                    if (type == ParticleTypeDead)
                        return; // we do not want to process dead particles
                    if (type == ParticleTypeBoundary)
                        return; // we do not want to process boundary particles

                    auto& iisph_data = data.collection->get<IISPHParticleData3D>(i);
                    auto& movement_data = data.collection->get<MovementData3D>(i);
                    auto& particle_data = data.collection->get<ParticleData>(i);

                    vec3 pressure_acceleration = vec3(0.0f);

                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors) {
                        auto type = data.collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead) {
                            continue; // don't calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = data.collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = data.collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = data.collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal) {
                            pressure_acceleration -=
                                    neighbor_particle_data.mass *
                                    (particle_data.pressure / Math::pow2(parameters.rest_density) +
                                            neighbor_particle_data.pressure / Math::pow2(parameters.rest_density)) *
                                    kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                            movement_data.position);
                        } else if (type == ParticleTypeBoundary) {
                            // TODO: gamma was used here even for multi layer boundaries
                            if (settings.single_layer_boundary) {
                                pressure_acceleration -= settings.gamma * neighbor_particle_data.mass * 2.0f *
                                        particle_data.pressure / Math::pow2(parameters.rest_density) *
                                        kernel.GetKernelDerivativeReversedValue(
                                                neighbor_movement_data.position, movement_data.position);
                            } else {
                                // TODO: check if we can ignore gamma in scenarios with multi layer boundaries
                                pressure_acceleration -= neighbor_particle_data.mass * 2.0f *
                                        particle_data.pressure / Math::pow2(parameters.rest_density) *
                                        kernel.GetKernelDerivativeReversedValue(
                                                neighbor_movement_data.position, movement_data.position);
                            }
                        }
                    }

                    movement_data.acceleration = pressure_acceleration;
                });


                // compute divergence of the velocity change, update pressure, compute predicted density error per
                // particle
                parallel::loop_for(0, data.collection->size(), [&](size_t i) {
                    auto type = data.collection->get<ParticleInfo>(i).type;
                    if (type == ParticleTypeDead)
                        return; // we do not want to process dead particles
                    if (type == ParticleTypeBoundary)
                        return; // we do not want to process boundary particles

                    auto& iisph_data = data.collection->get<IISPHParticleData3D>(i);
                    auto& movement_data = data.collection->get<MovementData3D>(i);
                    auto& particle_data = data.collection->get<ParticleData>(i);

                    float ap = 0.0f;

                    auto neighbors = neighborhood_search.get_neighbors(i);
                    for (auto neighbor : neighbors) {
                        auto type = data.collection->get<ParticleInfo>(neighbor).type;
                        if (type == ParticleTypeDead) {
                            continue; // don't calculate unnecessary values for dead particles.
                        }

                        auto& neighbor_particle_data = data.collection->get<ParticleData>(neighbor);
                        auto& neighbor_iisph_data = data.collection->get<IISPHParticleData3D>(neighbor);
                        auto& neighbor_movement_data = data.collection->get<MovementData3D>(neighbor);

                        if (type == ParticleTypeNormal) {
                            ap += neighbor_particle_data.mass *
                                    glm::dot(movement_data.acceleration - neighbor_movement_data.acceleration,
                                            kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                    movement_data.position));
                        } else if (type == ParticleTypeBoundary) {
                            ap += neighbor_particle_data.mass *
                                    glm::dot(movement_data.acceleration,
                                            kernel.GetKernelDerivativeReversedValue(neighbor_movement_data.position,
                                                    movement_data.position));
                        }
                    }

                    ap *= Math::pow2(current_timestep);

                    if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon()) {
                        particle_data.pressure =
                                std::fmax(0.0f, particle_data.pressure + settings.omega * ((iisph_data.source_term - ap) / iisph_data.diagonal_element));
                    } else {
                        // particle has no neighbors, hence set pressure to zero
                        particle_data.pressure = 0.0f;
                    }


                    // Info: The calculation of the particle density error is based on the implementation of IISPH by
                    // Stefan Band
                    float particle_density_error = std::abs(iisph_data.source_term - ap);
                    if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon()) {
                        if (std::abs(particle_data.pressure) <= std::numeric_limits<float>::epsilon()) {
                            // if the new pressure is zero, we do not have a density error
                            particle_density_error = 0.0f;
                        }

                        iisph_data.predicted_density_error = particle_density_error;
                    }
                });

                // log the iteration count until now for statistics
                this->stat_last_iteration_count = iteration + 1;

                // check if we need further iterations
                {
                    float max_predicted_density_error = 0.0f;
                    float average_predicted_density_error = 0.0f;
                    size_t average_counter = 0;

                    // reset the max final velocity and acceleration since values from previous iterations are not required anymore
                    max_final_velocity_squared = 0.0f;
                    max_final_acceleration_squared = 0.0f;

                    for (size_t i = 0; i < data.collection->size(); i++) {
                        // skip particles that are not normal fluid particles
                        auto type = data.collection->get<ParticleInfo>(i).type;
                        if (type != ParticleTypeNormal) {
                            continue;
                        }

                        auto& iisph_data = data.collection->get<IISPHParticleData3D>(i);

                        {
                            // calculate the current maximal velocity and acceleration of all fluid particles
                            const auto& movement_data = data.collection->get<MovementData3D>(i);

                            max_final_acceleration_squared = std::fmax(max_final_acceleration_squared, glm::dot(movement_data.acceleration, movement_data.acceleration));

                            vec3 particle_velocity = iisph_data.predicted_velocity + current_timestep * movement_data.acceleration;
                            max_final_velocity_squared = std::fmax(max_final_velocity_squared, glm::dot(particle_velocity, particle_velocity));
                        }

                        {
                            // calculate the max predicted density error for the iteration termination criteria
                            if (std::abs(iisph_data.diagonal_element) > std::numeric_limits<float>::epsilon()) {
                                // this particle contributes to the density error
                                max_predicted_density_error =
                                        std::fmax(max_predicted_density_error, iisph_data.predicted_density_error);
                                average_predicted_density_error += iisph_data.predicted_density_error;
                                average_counter++;
                            }
                        }
                    }

                    if (average_counter > 0) {
                        average_predicted_density_error = average_predicted_density_error / (float)average_counter;
                    }

                    // log the average predicted density error
                    this->stat_last_average_predicted_density_error = average_predicted_density_error;

                    // check termination criteria
                    if (iteration >= settings.min_number_of_iterations - 1) {
                        // we have at least reached the minimum number of iterations
                        if (average_predicted_density_error <= settings.max_density_error_allowed) {
                            // exit the solver, since the average predicted density error has become small enough
                            break;
                        }
                    }
                }
            }

            float old_timestep = current_timestep;
            // adapting timestep in order to not invalidate cfl condition
            {
                float max_final_velocity = sqrt(max_final_velocity_squared);
                float max_final_acceleration = sqrt(max_final_acceleration_squared);
                float corrected_timestep = data.timestep_generator->get_non_cfl_validating_timestep(max_final_acceleration, max_final_velocity);
                corrected_timestep = std::fmin(corrected_timestep, timestep.desired_time_step);
                FLUID_ASSERT(corrected_timestep > 0.0f);
                timestep.actual_time_step = corrected_timestep;
                current_timestep = timestep.actual_time_step;
            }


            // integrate particle movement
            parallel::loop_for(0, data.collection->size(), [&](size_t i) {
                // update velocity and position of all particles

                auto type = data.collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeBoundary) {
                    return; // don't calculate unnecessary values for the boundary particles.
                }
                if (type == ParticleTypeDead) {
                    return; // don't calculate unnecessary values for dead particles.
                }

                auto& movement_data = data.collection->get<MovementData3D>(i);
                const auto& pi = data.collection->get<IISPHParticleData3D>(i);
                vec3 predicted_velocity = pi.predicted_velocity;

                // correct predicted velocity if required
                if (old_timestep != current_timestep) {
                    predicted_velocity = predicted_velocity - old_timestep * pi.original_non_pressure_acceleration + current_timestep * pi.original_non_pressure_acceleration;
                }


                // integrate using euler cromer
                movement_data.velocity = predicted_velocity +
                        current_timestep * movement_data.acceleration;
                movement_data.position = movement_data.position + current_timestep * movement_data.velocity;
            });
        }


        virtual std::shared_ptr<NeighborhoodInterface> create_neighborhood_interface() override {
            return neighborhood_search.create_interface();
        }

        void create_compatibility_report(CompatibilityReport& report) override {
            initialize();

            report.begin_scope(FLUID_NAMEOF(IISPHFluidSolver3D));
            if (data.collection == nullptr) {
                report.add_issue("Particle collection is null.");
            } else {
                if (!data.collection->is_type_present<MovementData3D>()) {
                    report.add_issue("Particles are missing the MovementData3D attribute.");
                }
                if (!data.collection->is_type_present<ParticleData>()) {
                    report.add_issue("Particles are missing the ParticleData attribute.");
                }
                if (!data.collection->is_type_present<ParticleInfo>()) {
                    report.add_issue("Particles are missing the ParticleInfo attribute.");
                }
                if (!data.collection->is_type_present<ExternalForces3D>()) {
                    report.add_issue("Particles are missing the ExternalForces3D attribute.");
                }
                if (!data.collection->is_type_present<IISPHParticleData3D>()) {
                    report.add_issue("Particles are missing the IISPHParticleData3D attribute.");
                }
            }

            if (settings.max_number_of_iterations < settings.min_number_of_iterations) {
                report.add_issue("Max iterations are less than min number of iterations.");
            }

            if (settings.max_density_error_allowed <= 0.0f) {
                report.add_issue("MaxDensityErrorAllowed is smaller or equal to zero.");
            }

            if (data.timestep_generator == nullptr) {
                report.add_issue("Timestep generator is null");
            }
            neighborhood_search.create_compatibility_report(report);
            kernel.create_compatibility_report(report);

            report.end_scope();
        }
    };

} // namespace LibFluid