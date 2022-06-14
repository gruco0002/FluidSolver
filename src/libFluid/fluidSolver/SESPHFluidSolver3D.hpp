#pragma once

#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace FluidSolver {

    struct SESPHSettings3D : public DataChangeStruct {
        pFloat StiffnessK = 100000.0f;
        pFloat Viscosity = 5.0f;

        bool single_layer_boundary = false;
        float single_layer_boundary_gamma_1 = 2.0f;
        float single_layer_boundary_gamma_2 = 3.0f;
    };

    template<typename Kernel = CubicSplineKernel3D, typename NeighborhoodSearch = QuadraticNeighborhoodSearch3D,
            typename parallel = StdParallelForEach>
    class SESPHFluidSolver3D : public IFluidSolverBase {
      public:
        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        void execute_simulation_step(Timepoint& timestep) override;


        std::shared_ptr<NeighborhoodInterface> create_neighborhood_interface() override;

        void create_compatibility_report(CompatibilityReport& report) override;

        void initialize() override;

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


    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::execute_simulation_step(Timepoint& timestep) {
        initialize();

        FLUID_ASSERT(data.collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleData>());
        FLUID_ASSERT(data.collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(data.collection->is_type_present<ExternalForces3D>());

        FLUID_ASSERT(timestep.desired_time_step > 0.0f);

        FLUID_ASSERT(data.timestep_generator != nullptr);

        current_timestep = timestep.desired_time_step;

        // find neighbors for all particles
        FLUID_ASSERT(neighborhood_search.collection == data.collection);
        neighborhood_search.find_neighbors();

        // calculate density and pressure for all particles
        parallel::loop_for(0, data.collection->size(), [&](pIndex_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                return; // don't calculate unnecessary values for dead particles.
            }

            data.collection->get<ParticleData>(i).density = ComputeDensity(i);
            data.collection->get<ParticleData>(i).pressure = ComputePressure(i);
        });

        // compute non pressure accelerations and pressure accelerations for all particles
        parallel::loop_for(0, data.collection->size(), [&](pIndex_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                return; // don*t calculate unnecessary values for dead particles.
            }
            auto& mv = data.collection->get<MovementData3D>(i);

            vec3 nonPressureAcc = ComputeNonPressureAcceleration(i);
            vec3 pressureAcc = ComputePressureAcceleration(i);
            mv.acceleration = pressureAcc + nonPressureAcc;
        });

        // update velocity and position of all particles
        parallel::loop_for(0, data.collection->size(), [&](pIndex_t i) {
            auto type = data.collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                return; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                return; // don*t calculate unnecessary values for dead particles.
            }

            // integrate using euler cromer
            // FIXME: adapt timestep if required
            auto& mv = data.collection->get<MovementData3D>(i);
            mv.velocity = mv.velocity + current_timestep * mv.acceleration;
            mv.position = mv.position + current_timestep * mv.velocity;
        });
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::initialize() {
        FLUID_ASSERT(data.collection != nullptr);

        if (data.has_data_changed()) {
            data.acknowledge_data_change();
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

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    void SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(SESPHFluidSolver3D));
        if (data.collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
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
        }

        if (data.timestep_generator == nullptr) {
            report.add_issue("Timestep generator is null");
        }

        neighborhood_search.create_compatibility_report(report);
        kernel.create_compatibility_report(report);

        report.end_scope();
    }


    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    inline std::shared_ptr<NeighborhoodInterface> SESPHFluidSolver3D<Kernel, NeighborhoodSearch,
            parallel>::create_neighborhood_interface() {
        return neighborhood_search.create_interface();
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputePressure(pIndex_t particleIndex) {
        float density = data.collection->get<ParticleData>(particleIndex).density;
        float pressure = settings.StiffnessK * (density / parameters.rest_density - 1.0f);
        return std::max(pressure, 0.0f);
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    pFloat SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeDensity(pIndex_t particleIndex) {
        const vec3& position = data.collection->get<MovementData3D>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors) {
            auto type = data.collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead) {
                continue; // don't calculate unnecessary values for dead particles.
            }

            const vec3& neighborPosition = data.collection->get<MovementData3D>(neighbor).position;
            float neighborMass = data.collection->get<ParticleData>(neighbor).mass;
            if (type == ParticleTypeBoundary) {
                if (settings.single_layer_boundary) {
                    // we support single layer boundaries, scale the contribution of the boundary particles
                    density += settings.single_layer_boundary_gamma_1 * neighborMass * kernel.GetKernelValue(neighborPosition, position);
                } else {
                    density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
                }
            } else if (type == ParticleTypeNormal) {
                density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
            }
        }
        return density;
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeNonPressureAcceleration(
            pIndex_t particleIndex) {
        vec3 nonPressureAcceleration = vec3(0.0f);

        // Gravity
        nonPressureAcceleration += vec3(0.0f, -parameters.gravity, 0.0f);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    template<typename Kernel, typename NeighborhoodSearch, typename parallel>
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputePressureAcceleration(pIndex_t particleIndex) {
        const vec3& position = data.collection->get<MovementData3D>(particleIndex).position;
        const ParticleData& pData = data.collection->get<ParticleData>(particleIndex);
        const float& density = pData.density;
        const float& pressure = pData.pressure;
        const float& mass = pData.mass;

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / Math::pow2(density);

        vec3 pressureAcceleration = vec3(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor : neighbors) {
            auto type = data.collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const vec3& neighborPosition = data.collection->get<MovementData3D>(neighbor).position;
            if (type == ParticleTypeBoundary) {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                if (settings.single_layer_boundary) {
                    // scale the boundary contribution accordingly to support single layer boundaries
                    pressureAcceleration += -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                            kernel.GetKernelDerivativeReversedValue(neighborPosition, position) * settings.single_layer_boundary_gamma_2;

                } else {
                    pressureAcceleration += -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                            kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
                }
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
    vec3 SESPHFluidSolver3D<Kernel, NeighborhoodSearch, parallel>::ComputeViscosityAcceleration(pIndex_t particleIndex) {
        const vec3& position = data.collection->get<MovementData3D>(particleIndex).position;
        const vec3& velocity = data.collection->get<MovementData3D>(particleIndex).velocity;


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

        return 2.0f * settings.Viscosity * tmp;
    }


} // namespace FluidSolver