#pragma once

#include "core/fluidSolver/IFluidSolver.hpp"
#include "core/fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "core/parallelization/StdParallelForEach.hpp"

namespace FluidSolver
{

    struct SESPHSettings3D
    {
        pFloat StiffnessK = 100000.0f;
        pFloat Viscosity = 5.0f;
    };

    template <typename Kernel = CubicSplineKernel3D, typename NeighborhoodSearch = QuadraticNeighborhoodSearch3D,
              typename parallel = StdParallelForEach>
    class SESPHFluidSolver3D : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        void execute_simulation_step(pFloat timestep) override;

        void initialize() override;

        NeighborhoodInterface create_neighborhood_interface() override;

      private:
        pFloat current_timestep = 0.0f;

        pFloat ComputePressure(pIndex_t particleIndex);

        pFloat ComputeDensity(pIndex_t particleIndex);

        vec3 ComputeNonPressureAcceleration(pIndex_t particleIndex);

        vec3 ComputePressureAcceleration(pIndex_t particleIndex);

        vec3 ComputeViscosityAcceleration(pIndex_t particleIndex);


      public:
        SESPHSettings3D settings;
    };


    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::execute_simulation_step(pFloat timestep)
    {
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces3D>());


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
                return; // don't calculate unnecessary values for dead particles.
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

            vec3 nonPressureAcc = ComputeNonPressureAcceleration(i);
            vec3 pressureAcc = ComputePressureAcceleration(i);
            vec3 acceleration = pressureAcc + nonPressureAcc;
            collection->get<MovementData3D>(i).acceleration = acceleration;
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
            auto& mv = collection->get<MovementData3D>(i);
            mv.velocity = mv.velocity + current_timestep * mv.acceleration;
            mv.position = mv.position + current_timestep * mv.velocity;
        });
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::initialize()
    {
        FLUID_ASSERT(collection != nullptr);

        neighborhood_search.collection = collection;
        neighborhood_search.search_radius = parameters.particle_size * 2.0f;
        neighborhood_search.initialize();
        kernel.kernel_support = parameters.particle_size * 2.0f;

        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces3D>());
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    inline NeighborhoodInterface SESPHFluidSolver3D<Kernel, NeighborhoodSearch,
                                                    parallel>::create_neighborhood_interface()
    {
        return neighborhood_search.create_interface();
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputePressure(pIndex_t particleIndex)
    {
        float density = collection->get<ParticleData>(particleIndex).density;
        float pressure = settings.StiffnessK * (density / parameters.rest_density - 1.0f);
        return std::max(pressure, 0.0f);
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeDensity(pIndex_t particleIndex)
    {
        const vec3& position = collection->get<MovementData3D>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            const vec3& neighborPosition = collection->get<MovementData3D>(neighbor).position;
            float neighborMass = collection->get<ParticleData>(neighbor).mass;
            density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
        }
        return density;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeNonPressureAcceleration(
        pIndex_t particleIndex)
    {
        vec3 nonPressureAcceleration = vec3(0.0f);

        // Gravity
        nonPressureAcceleration += vec3(0.0f, -parameters.gravity, 0.0f);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    template <typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputePressureAcceleration(pIndex_t particleIndex)
    {
        const vec3& position = collection->get<MovementData3D>(particleIndex).position;
        const ParticleData& pData = collection->get<ParticleData>(particleIndex);
        const float& density = pData.density;
        const float& pressure = pData.pressure;
        const float& mass = pData.mass;

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / std::pow(density, 2.0f);

        vec3 pressureAcceleration = vec3(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors)
        {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead)
            {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const vec3& neighborPosition = collection->get<MovementData3D>(neighbor).position;
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
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeViscosityAcceleration(pIndex_t particleIndex)
    {
        const vec3& position = collection->get<MovementData3D>(particleIndex).position;
        const vec3& velocity = collection->get<MovementData3D>(particleIndex).velocity;


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

        return 2.0f * settings.Viscosity * tmp;
    }


} // namespace FluidSolver