#include <cstdint>
#include <algorithm>
#include <chrono>
#include "SESPHFluidSolver.hpp"

namespace FluidSolver {


    void SESPHFluidSolver::ExecuteSimulationStep() {
        CheckSolverIntegrity();

        auto t1 = std::chrono::high_resolution_clock::now();


        // find neighbors for all particles
        neighborhoodSearch->FindNeighbors();


        auto p1 = std::chrono::high_resolution_clock::now();
        // calculate density and pressure for all particles
#pragma omp parallel for
        for (int64_t i = 0; i < particleCollection->GetSize(); i++) {
            auto type = particleCollection->GetParticleType(i);
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
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
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
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
            if (type == ParticleTypeBoundary) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }
            if (type == ParticleTypeDead) {
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

    float SESPHFluidSolver::ComputePressure(uint32_t particleIndex) {
        float density = particleCollection->GetDensity(particleIndex);
        float pressure = StiffnessK * (density / RestDensity - 1.0f);
        return std::max(pressure, 0.0f);
    }

    float SESPHFluidSolver::ComputeDensity(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);

        float density = 0.0f;
        auto neighbors = neighborhoodSearch->GetNeighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }
            glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
            float neighborMass = particleCollection->GetMass(neighbor);
            density += neighborMass * kernel->GetKernelValue(neighborPosition, position);
        }
        return density;
    }

    glm::vec2 SESPHFluidSolver::ComputeNonPressureAcceleration(uint32_t particleIndex) {
        glm::vec2 nonPressureAcceleration = glm::vec2(0.0f);

        // Gravity
        nonPressureAcceleration += glm::vec2(0.0f, -Gravity);

        // Viscosity
        nonPressureAcceleration += ComputeViscosityAcceleration(particleIndex);

        return nonPressureAcceleration;
    }

    glm::vec2 SESPHFluidSolver::ComputePressureAcceleration(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        float density = particleCollection->GetDensity(particleIndex);
        float pressure = particleCollection->GetPressure(particleIndex);
        float mass = particleCollection->GetMass(particleIndex);

        float pressureDivDensitySquared = density == 0.0f ? 0.0f : pressure / std::pow(density, 2.0f);

        glm::vec2 pressureAcceleration = glm::vec2(0.0f);
        auto neighbors = neighborhoodSearch->GetNeighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == ParticleTypeDead) {
                continue; // don*t calculate unnecessary values for dead particles.
            }

            if (type == ParticleTypeBoundary) {
                // simple mirroring is used to calculate the pressure acceleration with a boundary particle
                glm::vec2 neighborPosition = particleCollection->GetPosition(neighbor);
                pressureAcceleration +=
                        -mass * (pressureDivDensitySquared + pressureDivDensitySquared) *
                        kernel->GetKernelDerivativeReversedValue(neighborPosition, position);

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
                        kernel->GetKernelDerivativeReversedValue(neighborPosition, position);
            }

        }
        return pressureAcceleration;
    }

    glm::vec2 SESPHFluidSolver::ComputeViscosityAcceleration(uint32_t particleIndex) {
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        glm::vec2 velocity = particleCollection->GetVelocity(particleIndex);


        glm::vec2 tmp = glm::vec2(0.0f);
        auto neighbors = neighborhoodSearch->GetNeighbors(particleIndex);
        for (uint32_t neighbor: neighbors) {
            auto type = particleCollection->GetParticleType(neighbor);
            if (type == ParticleTypeDead) {
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
                   kernel->GetKernelDerivativeReversedValue(neighborPosition, position);


        }

        return 2.0f * Viscosity * tmp;
    }

    SESPHFluidSolver::~SESPHFluidSolver() {
        delete neighborhoodSearch;
        neighborhoodSearch = nullptr;
    }

    float SESPHFluidSolver::getParticleSize() {
        return ParticleSize;
    }

    void SESPHFluidSolver::setParticleSize(float particleSize) {
        this->ParticleSize = particleSize;
    }

    float SESPHFluidSolver::getRestDensity() {
        return RestDensity;
    }

    void SESPHFluidSolver::setRestDensity(float restDensity) {
        this->RestDensity = restDensity;
    }

    float SESPHFluidSolver::getTimestep() {
        return TimeStep;
    }

    void SESPHFluidSolver::setTimestep(float timestep) {
        this->TimeStep = timestep;
    }

    void SESPHFluidSolver::setParticleCollection(IParticleCollection *particleCollection) {
        this->particleCollection = particleCollection;
    }

    IParticleCollection *SESPHFluidSolver::getParticleCollection() {
        return particleCollection;
    }

    float SESPHFluidSolver::getGravity() {
        return Gravity;
    }

    void SESPHFluidSolver::setGravity(float gravity) {
        this->Gravity = gravity;
    }

    uint32_t SESPHFluidSolver::GetComputationTimeLastTimestepInMicroseconds() {
        return compTimeTotalMicroseconds;
    }

    uint32_t SESPHFluidSolver::GetComputationTimePressureSolverLastTimestepInMicroseconds() {
        return compTimePressureSolverMicroseconds;
    }

    void SESPHFluidSolver::SetKernel(IKernel *kernel) {
        this->kernel = kernel;
    }

    IKernel *SESPHFluidSolver::GetKernel() {
        return this->kernel;
    }

    void SESPHFluidSolver::SetNeighborhoodSearch(INeighborhoodSearch *neighborhoodSearch) {
        this->neighborhoodSearch = neighborhoodSearch;
    }

    INeighborhoodSearch *SESPHFluidSolver::GetNeighborhoodSearch() {
        return this->neighborhoodSearch;
    }
}