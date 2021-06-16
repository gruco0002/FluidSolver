#ifndef FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SESPHFLUIDSOLVER_HPP


#include <fluidSolver/IFluidSolver.hpp>
#include <fluidSolver/kernel/CubicSplineKernel.hpp>
#include <fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <parallelization/StdParallelForEach.hpp>

namespace FluidSolver
{

    struct SESPHSettings
    {
        pFloat StiffnessK = 100000.0f;
        pFloat Viscosity = 5.0f;
    };

    template <typename Kernel = CubicSplineKernel,
              typename NeighborhoodSearch = QuadraticNeighborhoodSearchDynamicAllocated,
              typename parallel = StdParallelForEach>
    class SESPHFluidSolver : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        void execute_simulation_step(pFloat timestep) override;

        void initialize() override;

        NeighborhoodInterface create_neighborhood_interface() override;

        Compatibility check() override;

      private:
        pFloat current_timestep = 0.0f;

        pFloat ComputePressure(pIndex_t particleIndex);

        pFloat ComputeDensity(pIndex_t particleIndex);

        vec2 ComputeNonPressureAcceleration(pIndex_t particleIndex);

        vec2 ComputePressureAcceleration(pIndex_t particleIndex);

        vec2 ComputeViscosityAcceleration(pIndex_t particleIndex);


      public:
        SESPHSettings settings;
    };

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::execute_simulation_step(pFloat timestep)
    {
        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces>());


        FLUID_ASSERT(timestep > 0.0f);
        current_timestep = timestep;

        // find neighbors for all particles
        FLUID_ASSERT(neighborhood_search.collection == collection);
        neighborhood_search.find_neighbors();

        // calculate density and pressure for all particles
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary)
            {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead)
            {
                return; // don*t calculate unnecessary values for dead particles.
            }

            collection->get<ParticleData>(i).density = ComputeDensity(i);


            collection->get<ParticleData>(i).pressure = ComputePressure(i);
        });

        // compute non pressure accelerations and pressure accelerations for all particles
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary)
            {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead)
            {
                return; // don*t calculate unnecessary values for dead particles.
            }

            glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
            glm::vec2 pressureAcc = ComputePressureAcceleration(i);
            glm::vec2 acceleration = pressureAcc + nonPressureAcc;
            collection->get<MovementData>(i).acceleration = acceleration;
        });

        // update velocity and position of all particles
        parallel::loop_for(0, collection->size(), [&](pIndex_t i) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary)
            {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead)
            {
                return; // don*t calculate unnecessary values for dead particles.
            }

            // integrate using euler cromer
            collection->get<MovementData>(i).velocity =
                collection->get<MovementData>(i).velocity +
                current_timestep * collection->get<MovementData>(i).acceleration;
            collection->get<MovementData>(i).position = collection->get<MovementData>(i).position +
                                                        current_timestep * collection->get<MovementData>(i).velocity;
        });
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::initialize()
    {
        FLUID_ASSERT(collection != nullptr);

        neighborhood_search.collection = collection;
        neighborhood_search.search_radius = parameters.particle_size * 2.0f;
        neighborhood_search.initialize();
        kernel.kernel_support = parameters.particle_size * 2.0f;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    Compatibility SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::check()
    {
        Compatibility c;
        if (collection == nullptr)
        {
            c.add_issue({"SESPHFluidSolver", "ParticleCollection is null."});
        }
        else
        {
            if (!collection->is_type_present<MovementData>())
            {
                c.add_issue({"SESPHFluidSolver", "Particles are missing the MovementData attribute."});
            }
            if (!collection->is_type_present<ParticleData>())
            {
                c.add_issue({"SESPHFluidSolver", "Particles are missing the ParticleData attribute."});
            }
            if (!collection->is_type_present<ParticleInfo>())
            {
                c.add_issue({"SESPHFluidSolver", "Particles are missing the ParticleInfo attribute."});
            }
            if (!collection->is_type_present<ExternalForces>())
            {
                c.add_issue({"SESPHFluidSolver", "Particles are missing the ExternalForces attribute."});
            }
        }


        c.add_compatibility(neighborhood_search.check());
        c.add_compatibility(kernel.check());

        return c;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    inline NeighborhoodInterface SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::create_neighborhood_interface()
    {
        return neighborhood_search.create_interface();
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputePressure(pIndex_t particleIndex)
    {
        float density = collection->get<ParticleData>(particleIndex).density;
        float pressure = settings.StiffnessK * (density / parameters.rest_density - 1.0f);
        return std::max(pressure, 0.0f);
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeDensity(pIndex_t particleIndex)
    {
        const glm::vec2& position = collection->get<MovementData>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            const glm::vec2& neighborPosition = collection->get<MovementData>(neighbor).position;
            float neighborMass = collection->get<ParticleData>(neighbor).mass;
            density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
        }
        return density;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeNonPressureAcceleration(pIndex_t particleIndex)
    {
        glm::vec2 nonPressureAcceleration = glm::vec2(0.0f);

        // Gravity
        nonPressureAcceleration += glm::vec2(0.0f, -parameters.gravity);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputePressureAcceleration(pIndex_t particleIndex)
    {
        const glm::vec2& position = collection->get<MovementData>(particleIndex).position;
        const ParticleData& pData = collection->get<ParticleData>(particleIndex);
        const float& density = pData.density;
        const float& pressure = pData.pressure;
        const float& mass = pData.mass;

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / std::pow(density, 2.0f);

        glm::vec2 pressureAcceleration = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const glm::vec2& neighborPosition = collection->get<MovementData>(neighbor).position;
            if (type == ParticleTypeBoundary)
            {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                pressureAcceleration += -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            }
            else
            {
                // normal particles
                const ParticleData& neighbor_pData = collection->get<ParticleData>(neighbor);
                const float& neighborMass = neighbor_pData.mass;
                const float& neighborDensity = neighbor_pData.density;
                const float& neighborPressure = neighbor_pData.pressure;

                float neighborPressureDivDensitySquared =
                    neighborDensity == 0.0f ? 0.0f : neighborPressure / std::pow(neighborDensity, 2.0f);

                pressureAcceleration += -neighborMass *
                                        (pressureDivDensitySquared + neighborPressureDivDensitySquared) *
                                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            }
        }
        return pressureAcceleration;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeViscosityAcceleration(pIndex_t particleIndex)
    {
        const glm::vec2& position = collection->get<MovementData>(particleIndex).position;
        const glm::vec2& velocity = collection->get<MovementData>(particleIndex).velocity;


        glm::vec2 tmp = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const glm::vec2& neighborPosition = collection->get<MovementData>(neighbor).position;
            const glm::vec2& neighborVelocity = collection->get<MovementData>(neighbor).velocity;
            float neighborMass = collection->get<ParticleData>(neighbor).mass;
            float neighborDensity = collection->get<ParticleData>(neighbor).density;

            if (neighborDensity == 0.0f)
                continue;

            glm::vec2 vij = velocity - neighborVelocity;
            glm::vec2 xij = position - neighborPosition;

            tmp += (neighborMass / neighborDensity) *
                   (glm::dot(vij, xij) /
                    (glm::dot(xij, xij) + 0.01f * parameters.particle_size * parameters.particle_size)) *
                   kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
        }

        return 2.0f * settings.Viscosity * tmp;
    }
} // namespace FluidSolver


#endif // FLUIDSOLVER_SESPHFLUIDSOLVER_HPP