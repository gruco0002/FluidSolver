#include <cstdint>
#include <algorithm>
#include "SESPHFluidSolver.hpp"

namespace FluidSolver {

    float SESPHFluidSolver::ComputePressure(uint32_t particleIndex) {
        float density = collection->get<ParticleData>(particleIndex).density;
        float pressure = settings.StiffnessK * (density / parameters.rest_density - 1.0f);
        return std::max(pressure, 0.0f);
    }

    float SESPHFluidSolver::ComputeDensity(uint32_t particleIndex) {
        const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;

        float density = 0.0f;
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            const glm::vec2 &neighborPosition = collection->get<MovementData>(neighbor).position;
            float neighborMass = collection->get<ParticleData>(neighbor).mass;
            density += neighborMass * kernel.GetKernelValue(neighborPosition, position);
        }
        return density;
    }

    glm::vec2 SESPHFluidSolver::ComputeNonPressureAcceleration(uint32_t particleIndex) {
        glm::vec2 nonPressureAcceleration = glm::vec2(0.0f);

        // Gravity
        nonPressureAcceleration += glm::vec2(0.0f, -parameters.gravity);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    glm::vec2 SESPHFluidSolver::ComputePressureAcceleration(uint32_t particleIndex) {
        const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;
        const ParticleData &pData = collection->get<ParticleData>(particleIndex);
        const float &density = pData.density;
        const float &pressure = pData.pressure;
        const float &mass = pData.mass;

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / std::pow(density, 2.0f);

        glm::vec2 pressureAcceleration = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const glm::vec2 &neighborPosition = collection->get<MovementData>(neighbor).position;
            if (type == ParticleTypeBoundary) {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                pressureAcceleration +=
                        -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);

            } else {
                // normal particles
                const ParticleData &neighbor_pData = collection->get<ParticleData>(neighbor);
                const float &neighborMass = neighbor_pData.mass;
                const float &neighborDensity = neighbor_pData.density;
                const float &neighborPressure = neighbor_pData.pressure;

                float neighborPressureDivDensitySquared =
                        neighborDensity == 0.0f ? 0.0f : neighborPressure / std::pow(neighborDensity, 2.0f);

                pressureAcceleration +=
                        -neighborMass * (pressureDivDensitySquared + neighborPressureDivDensitySquared) *
                        kernel.GetKernelDerivativeReversedValue(neighborPosition, position);
            }

        }
        return pressureAcceleration;
    }

    glm::vec2 SESPHFluidSolver::ComputeViscosityAcceleration(uint32_t particleIndex) {
        const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;
        const glm::vec2 &velocity = collection->get<MovementData>(particleIndex).velocity;


        glm::vec2 tmp = glm::vec2(0.0f);
        auto neighbors = neighborhood_search.get_neighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = collection->get<ParticleInfo>(neighbor).type;
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            const glm::vec2 &neighborPosition = collection->get<MovementData>(neighbor).position;
            const glm::vec2 &neighborVelocity = collection->get<MovementData>(neighbor).velocity;
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

    void SESPHFluidSolver::execute_simulation_step(float timestep) {
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
#pragma omp parallel for
        for (int64_t i = 0; i < collection->size(); i++) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            collection->get<ParticleData>(i).density = ComputeDensity(i);


            collection->get<ParticleData>(i).pressure = ComputePressure(i);

        }

        // compute non pressure accelerations and pressure accelerations for all particles
#pragma omp parallel for
        for (int64_t i = 0; i < collection->size(); i++) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
            glm::vec2 pressureAcc = ComputePressureAcceleration(i);
            glm::vec2 acceleration = pressureAcc + nonPressureAcc;
            collection->get<MovementData>(i).acceleration = acceleration;
        }

        // update velocity and position of all particles
#pragma omp parallel for
        for (int64_t i = 0; i < collection->size(); i++) {
            auto type = collection->get<ParticleInfo>(i).type;
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            // integrate using euler cromer
            collection->get<MovementData>(i).velocity = collection->get<MovementData>(i).velocity + current_timestep *
                                                                                                    collection->get<MovementData>(
                                                                                                            i).acceleration;
            collection->get<MovementData>(i).position = collection->get<MovementData>(i).position +
                                                        current_timestep * collection->get<MovementData>(i).velocity;

        }


    }

    void SESPHFluidSolver::initialize() {
        FLUID_ASSERT(collection != nullptr);

        neighborhood_search.collection = collection;
        neighborhood_search.search_radius = parameters.particle_size * 2.0f;
        neighborhood_search.initialize();
        kernel.kernel_support = parameters.particle_size * 2.0f;

        FLUID_ASSERT(collection->is_type_present<MovementData>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ExternalForces>());
    }


}