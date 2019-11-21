//
// Created by corbi on 20.11.2019.
//

#include "IISPHFluidSolver.hpp"

void FluidSolver::IISPHFluidSolver::ExecuteSimulationStep() {

 /*   // find neighbors for all particles
    neighborhoodSearch->SetParticleCount(ParticleCollection->GetSize());
    neighborhoodSearch->FindNeighbors(ParticleCollection, NeighborhoodRadius);


    // calculate density and pressure for all particles
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        auto type = ParticleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        float density = ComputeDensity(i);
        ParticleCollection->SetDensity(i, density);

        float pressure = ComputePressure(i);
        ParticleCollection->SetPressure(i, pressure);
    }

    // compute non pressure accelerations and pressure accelerations for all particles
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        auto type = ParticleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
        glm::vec2 pressureAcc = ComputePressureAcceleration(i);
        glm::vec2 acceleration = pressureAcc + nonPressureAcc;
        ParticleCollection->SetAcceleration(i, acceleration);
    }

    // update velocity and position of all particles
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        auto type = ParticleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        // integrate using euler cromer
        glm::vec2 acceleration = ParticleCollection->GetAcceleration(i);
        glm::vec2 velocity = ParticleCollection->GetVelocity(i) + Timestep * acceleration;
        glm::vec2 position = ParticleCollection->GetPosition(i) + Timestep * velocity;

        ParticleCollection->SetVelocity(i, velocity);
        ParticleCollection->SetPosition(i, position);
    }

*/
}

float FluidSolver::IISPHFluidSolver::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::IISPHFluidSolver::setParticleSize(float particleSize) {
    if (this->ParticleSize != particleSize || neighborhoodSearch == nullptr || kernel == nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = new HashedNeighborhoodSearch(particleSize * 3);
    }
    this->ParticleSize = particleSize;
    this->KernelSupport = 2.0f * particleSize;
    this->NeighborhoodRadius = 2.0f * particleSize;
}

float FluidSolver::IISPHFluidSolver::getRestDensity() {
    return RestDensity;
}

void FluidSolver::IISPHFluidSolver::setRestDensity(float restDensity) {
    this->RestDensity = restDensity;
}

float FluidSolver::IISPHFluidSolver::getTimestep() {
    return Timestep;
}

void FluidSolver::IISPHFluidSolver::setTimestep(float timestep) {
    this->Timestep = timestep;
}

void FluidSolver::IISPHFluidSolver::setParticleCollection(FluidSolver::IParticleCollection *ParticleCollection) {
    this->ParticleCollection = ParticleCollection;
}

FluidSolver::IParticleCollection *FluidSolver::IISPHFluidSolver::getParticleCollection() {
    return ParticleCollection;
}

float FluidSolver::IISPHFluidSolver::getGravity() {
    return Gravity;
}

void FluidSolver::IISPHFluidSolver::setGravity(float gravity) {
    this->Gravity = gravity;
}
