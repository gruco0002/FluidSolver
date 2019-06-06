//
// Created by corbi on 24.04.2019.
//

#include <cstdint>
#include <algorithm>
#include "SPHFluidSolver.hpp"

namespace FluidSolver {


    void SPHFluidSolver::ExecuteSimulationStep() {

        // find neighbors for all particles
        neighborhoodSearch->SetParticleCount(particleCollection->GetSize());
        neighborhoodSearch->FindNeighbors(particleCollection, NeighborhoodRadius);


        // calculate density and pressure for all particles
        #pragma omp parallel for
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            auto type = particleCollection->GetParticleType(i);
            if (type == IParticleCollection::ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            float density = ComputeDensity(i);
            particleCollection->SetDensity(i, density);

            float pressure = ComputePressure(i);
            particleCollection->SetPressure(i, pressure);
        }

        // compute non pressure accelerations and pressure accelerations for all particles
        #pragma omp parallel for
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            auto type = particleCollection->GetParticleType(i);
            if (type == IParticleCollection::ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
            glm::vec2 pressureAcc = ComputePressureAcceleration(i);
            glm::vec2 acceleration = pressureAcc + nonPressureAcc;
            particleCollection->SetAcceleration(i, acceleration);
        }

        // update velocity and position of all particles
        #pragma omp parallel for
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            auto type = particleCollection->GetParticleType(i);
            if (type == IParticleCollection::ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            integrationScheme->Integrate(i, particleCollection, TimeStep);
        }

        // modify the simulation
        ISimulationModifier::SimulationInfo info;
        info.restDensity = RestDensity;
        info.timeStep = TimeStep;
        info.particleSize = ParticleSize;
        for (ISimulationModifier *modifier : simulationModifiers) {
            modifier->ModifySimulation(particleCollection, info);
        }

    }

    float SPHFluidSolver::ComputePressure(uint32_t particleIndex) {
        float density = particleCollection->GetDensity(particleIndex);
        float pressure = StiffnessK * (density / RestDensity - 1.0f);
        return std::max(pressure, 0.0f);
    }

    float SPHFluidSolver::ComputeDensity(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);

        float density = 0.0f;
        for (uint32_t neighbor: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
            float neighborMass = particleCollection->GetMass(neighbor);
            density += neighborMass * kernel->GetKernelValue(neighborPosition, position, KernelSupport);
        }
        return density;
    }

    glm::vec2 SPHFluidSolver::ComputeNonPressureAcceleration(uint32_t particleIndex) {
        glm::vec2 nonPressureAcceleration = glm::vec2(0.0f);

        // Gravity
        nonPressureAcceleration += glm::vec2(0.0f, -Gravity);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    glm::vec2 SPHFluidSolver::ComputePressureAcceleration(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        float density = particleCollection->GetDensity(particleIndex);
        float pressure = particleCollection->GetPressure(particleIndex);
        float mass = particleCollection->GetMass(particleIndex);

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / std::pow(density, 2.0f);

        glm::vec2 pressureAcceleration = glm::vec2(0.0f);
        for (uint32_t neighbor: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            if (type == IParticleCollection::ParticleTypeBoundary) {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
                pressureAcceleration +=
                        -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                        kernel->GetKernelDerivativeReversedValue(neighborPosition, position, KernelSupport);

            } else {
                // normal particles
                glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
                float neighborMass = particleCollection->GetMass(neighbor);
                float neighborDensity = particleCollection->GetDensity(neighbor);
                float neighborPressure = particleCollection->GetPressure(neighbor);

                float neighborPressureDivDensitySquared =
                        neighborDensity == 0.0f ? 0.0f : neighborPressure / std::pow(neighborDensity, 2.0f);

                pressureAcceleration +=
                        -neighborMass * (pressureDivDensitySquared + neighborPressureDivDensitySquared) *
                        kernel->GetKernelDerivativeReversedValue(neighborPosition, position, KernelSupport);
            }

        }
        return pressureAcceleration;
    }

    glm::vec2 SPHFluidSolver::ComputeViscosityAcceleration(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        glm::vec2 velocity = particleCollection->GetVelocity(particleIndex);


        glm::vec2 tmp = glm::vec2(0.0f);
        for (uint32_t neighbor: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
            glm::vec2 neighborVelocity = particleCollection->GetVelocity(neighbor);
            float neighborMass = particleCollection->GetMass(neighbor);
            float neighborDensity = particleCollection->GetDensity(neighbor);

            if (neighborDensity == 0.0f)
                continue;

            glm::vec2 vij = velocity - neighborVelocity;
            glm::vec2 xij = position - neighborPosition;

            tmp += (neighborMass / neighborDensity) *
                   (glm::dot(vij, xij) / (glm::dot(xij, xij) + 0.01f * ParticleSize * ParticleSize)) *
                   kernel->GetKernelDerivativeReversedValue(neighborPosition, position, KernelSupport);


        }

        return 2.0f * Viscosity * tmp;
    }
}