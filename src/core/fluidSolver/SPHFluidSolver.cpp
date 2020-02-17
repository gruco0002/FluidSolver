//
// Created by corbi on 24.04.2019.
//

#include <cstdint>
#include <algorithm>
#include <chrono>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include "SPHFluidSolver.hpp"

namespace FluidSolver {


    void SPHFluidSolver::ExecuteSimulationStep() {
        auto t1 = std::chrono::high_resolution_clock::now();


        // find neighbors for all particles
        neighborhoodSearch->SetParticleCount(particleCollection->GetSize());
        neighborhoodSearch->FindNeighbors(particleCollection, NeighborhoodRadius);


        auto p1 = std::chrono::high_resolution_clock::now();
        // calculate density and pressure for all particles
#pragma omp parallel for
        for (int64_t i = 0; i < particleCollection->GetSize(); i++) {
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
        auto p2 = std::chrono::high_resolution_clock::now();

        // compute non pressure accelerations and pressure accelerations for all particles
#pragma omp parallel for
        for (int64_t i = 0; i < particleCollection->GetSize(); i++) {
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
        for (int64_t i = 0; i < particleCollection->GetSize(); i++) {
            auto type = particleCollection->GetParticleType(i);
            if (type == IParticleCollection::ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == IParticleCollection::ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            // integrate using euler cromer
            glm::vec2 acceleration = particleCollection->GetAcceleration(i);
            glm::vec2 velocity = particleCollection->GetVelocity(i) + TimeStep * acceleration;
            glm::vec2 position = particleCollection->GetPosition(i) + TimeStep * velocity;

            particleCollection->SetVelocity(i, velocity);
            particleCollection->SetPosition(i, position);
        }

        auto t2 = std::chrono::high_resolution_clock::now();
        this->compTimeTotalMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        this->compTimePressureSolverMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                p2 - p1).count();
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

    SPHFluidSolver::~SPHFluidSolver() {

    }

    float SPHFluidSolver::getParticleSize() {
        return ParticleSize;
    }

    void SPHFluidSolver::setParticleSize(float particleSize) {
        if (this->ParticleSize != particleSize || neighborhoodSearch == nullptr) {
            delete neighborhoodSearch;
            neighborhoodSearch = new FluidSolver::HashedNeighborhoodSearch(getParticleSize() * 3.0f);
        }
        this->ParticleSize = particleSize;
        KernelSupport = 2.0f * this->ParticleSize;
        NeighborhoodRadius = 2.0f * this->ParticleSize;
    }

    float SPHFluidSolver::getRestDensity() {
        return RestDensity;
    }

    void SPHFluidSolver::setRestDensity(float restDensity) {
        this->RestDensity = restDensity;
    }

    float SPHFluidSolver::getTimestep() {
        return TimeStep;
    }

    void SPHFluidSolver::setTimestep(float timestep) {
        this->TimeStep = timestep;
    }

    void SPHFluidSolver::setParticleCollection(IParticleCollection *particleCollection) {
        if (neighborhoodSearch != nullptr) {
            delete neighborhoodSearch;
            neighborhoodSearch = new FluidSolver::HashedNeighborhoodSearch(getParticleSize() * 3.0f);
        }
        this->particleCollection = particleCollection;
    }

    IParticleCollection *SPHFluidSolver::getParticleCollection() {
        return particleCollection;
    }

    float SPHFluidSolver::getGravity() {
        return Gravity;
    }

    void SPHFluidSolver::setGravity(float gravity) {
        this->Gravity = gravity;
    }

    SPHFluidSolver::SPHFluidSolver() {

    }

    uint32_t SPHFluidSolver::GetComputationTimeLastTimestepInMicroseconds() {
        return compTimeTotalMicroseconds;
    }

    uint32_t SPHFluidSolver::GetComputationTimePressureSolverLastTimestepInMicroseconds() {
        return compTimePressureSolverMicroseconds;
    }
}