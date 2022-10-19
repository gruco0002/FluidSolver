#pragma once

#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/kernel/CubicSplineKernel.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"
#include "fluidSolver/solver/settings/SESPHSettings.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace LibFluid {


    template<typename Kernel = CubicSplineKernel,
            typename NeighborhoodSearch = QuadraticNeighborhoodSearchDynamicAllocated,
            typename parallel = StdParallelForEach>
    class SESPHFluidSolver : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        void execute_simulation_step(Timepoint& timestep) override;

        std::shared_ptr<NeighborhoodInterface> create_neighborhood_interface() override;

        void create_compatibility_report(CompatibilityReport& report) override;

        void initialize() override;
        void execute_neighborhood_search() override;

      private:
        float current_timestep = 0.0f;

        float ComputePressure(size_t particleIndex);

        float ComputeDensity(size_t particleIndex);

        glm::vec2 ComputeNonPressureAcceleration(size_t particleIndex);

        glm::vec2 ComputePressureAcceleration(size_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(size_t particleIndex);


      public:
        SESPHSettings settings;
    };

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::execute_simulation_step(Timepoint& timestep) {
        initialize();

        FLUID_ASSERT(data.collection->is_type_present<MovementData>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleData>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(data.collection->is_type_present<ExternalForces>());

        FLUID_ASSERT(timestep.desired_time_step > 0.0f);

        FLUID_ASSERT(data.timestep_generator != nullptr);

        current_timestep = timestep.desired_time_step;


        // calculate density and pressure for all particles
        parallel::loop_for(0, data.collection->size(), [&](size_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeInactive) {
                return; // don*t calculate unnecessary values for inactive particles.
            }

            data.collection->get<ParticleData>(i).density = ComputeDensity(i);


            data.collection->get<ParticleData>(i).pressure = ComputePressure(i);
        });

        // compute non pressure accelerations and pressure accelerations for all particles
        parallel::loop_for(0, data.collection->size(), [&](size_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeInactive) {
                return; // don*t calculate unnecessary values for inactive particles.
            }

            glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
            glm::vec2 pressureAcc = ComputePressureAcceleration(i);
            glm::vec2 acceleration = pressureAcc + nonPressureAcc;
            data.collection->get<MovementData>(i).acceleration = acceleration;
        });

        // update velocity and position of all particles
        parallel::loop_for(0, data.collection->size(), [&](size_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeInactive) {
                return; // don*t calculate unnecessary values for inactive particles.
            }

            // integrate using euler cromer
            // FIXME: adapt timestep if required
            data.collection->get<MovementData>(i).velocity =
                    data.collection->get<MovementData>(i).velocity +
                    current_timestep * data.collection->get<MovementData>(i).acceleration;
            data.collection->get<MovementData>(i).position = data.collection->get<MovementData>(i).position +
                    current_timestep * data.collection->get<MovementData>(i).velocity;
        });
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::initialize() {
        FLUID_ASSERT(data.collection != nullptr);

        if (data.has_data_changed()) {
            data.acknowledge_data_change();
            neighborhood_search.collection = data.collection;
            neighborhood_search.initialize();
        }

        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();
            neighborhood_search.search_radius = parameters.particle_size * Math::kernel_support_factor;
            neighborhood_search.initialize();
            kernel.kernel_support = parameters.particle_size * Math::kernel_support_factor;
            kernel.initialize();
        }

        if (settings.has_data_changed()) {
            settings.acknowledge_data_change();
        }
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(SESPHFluidSolver));

        if (data.collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!data.collection->is_type_present<MovementData>()) {
                report.add_issue("Particles are missing the MovementData attribute.");
            }
            if (!data.collection->is_type_present<ParticleData>()) {
                report.add_issue("Particles are missing the ParticleData attribute.");
            }
            if (!data.collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
            if (!data.collection->is_type_present<ExternalForces>()) {
                report.add_issue("Particles are missing the ExternalForces attribute.");
            }
        }

        if (data.timestep_generator == nullptr) {
            report.add_issue("Timestep generator is null");
        }

        neighborhood_search.create_compatibility_report(report);
        kernel.create_compatibility_report(report);

        report.end_scope();
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    inline std::shared_ptr<NeighborhoodInterface> SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::create_neighborhood_interface() {
        return neighborhood_search.create_interface();
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    float SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputePressure(size_t particleIndex) {
        float density = data.collection->get<ParticleData>(particleIndex).density;
        float pressure = settings.StiffnessK * (density / parameters.rest_density - 1.0f);
        return std::max(pressure, 0.0f);
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    float SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeDensity(size_t particleIndex) {
        const glm::vec2& position = data.collection->get<MovementData>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors) {
            auto type = data.collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeInactive) {
                continue; // don*t calculate unnecessary values for inactive particles.
            }
            const glm::vec2& neighborPosition = data.collection->get<MovementData>(neighbor).position;
            float neighborMass = data.collection->get<ParticleData>(neighbor).mass;
            density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
        }
        return density;
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    glm::vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeNonPressureAcceleration(size_t particleIndex) {
        glm::vec2 nonPressureAcceleration = glm::vec2(0.0f);

        // Gravity
        nonPressureAcceleration += glm::vec2(0.0f, -parameters.gravity);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    glm::vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputePressureAcceleration(size_t particleIndex) {
        const glm::vec2& position = data.collection->get<MovementData>(particleIndex).position;
        const ParticleData& pData = data.collection->get<ParticleData>(particleIndex);
        const float& density = pData.density;
        const float& pressure = pData.pressure;
        const float& mass = pData.mass;

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / Math::pow2(density);

        glm::vec2 pressureAcceleration = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors) {
            auto type = data.collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeInactive) {
                continue; // don*t calculate unnecessary values for inactive particles.
            }

            const glm::vec2& neighborPosition = data.collection->get<MovementData>(neighbor).position;
            if (type == ParticleTypeBoundary) {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                pressureAcceleration += -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            } else {
                // normal particles
                const ParticleData& neighbor_pData = data.collection->get<ParticleData>(neighbor);
                const float& neighborMass = neighbor_pData.mass;
                const float& neighborDensity = neighbor_pData.density;
                const float& neighborPressure = neighbor_pData.pressure;

                float neighborPressureDivDensitySquared =
                        neighborDensity == 0.0f ? 0.0f : neighborPressure / Math::pow2(neighborDensity);

                pressureAcceleration += -neighborMass *
                        (pressureDivDensitySquared + neighborPressureDivDensitySquared) *
                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            }
        }
        return pressureAcceleration;
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    glm::vec2 SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::ComputeViscosityAcceleration(size_t particleIndex) {
        const glm::vec2& position = data.collection->get<MovementData>(particleIndex).position;
        const glm::vec2& velocity = data.collection->get<MovementData>(particleIndex).velocity;


        glm::vec2 tmp = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors) {
            auto type = data.collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeInactive) {
                continue; // don*t calculate unnecessary values for inactive particles.
            }

            const glm::vec2& neighborPosition = data.collection->get<MovementData>(neighbor).position;
            const glm::vec2& neighborVelocity = data.collection->get<MovementData>(neighbor).velocity;
            float neighborMass = data.collection->get<ParticleData>(neighbor).mass;
            float neighborDensity = data.collection->get<ParticleData>(neighbor).density;

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
    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver<Kernel, NeighborhoodSearch, parallel>::execute_neighborhood_search() {
        initialize();

        FLUID_ASSERT(data.collection->is_type_present<MovementData>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleData>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(data.collection->is_type_present<ExternalForces>());

        // find neighbors for all particles
        FLUID_ASSERT(neighborhood_search.collection == data.collection);
        neighborhood_search.find_neighbors();
    }
} // namespace LibFluid
