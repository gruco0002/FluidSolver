//
// Created by corbi on 20.11.2019.
//

#include "IISPHFluidSolver.hpp"

void FluidSolver::IISPHFluidSolver::ExecuteSimulationStep() {

    // find neighbors for all particles
    neighborhoodSearch->SetParticleCount(ParticleCollection->GetSize());
    neighborhoodSearch->FindNeighbors(ParticleCollection, NeighborhoodRadius);

    // calculating density and non pressure accelerations
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        CalculateDensity(i);
        CalculateNonPressureAccelerationAndPredictedVelocity(i);
    }

    // compute source term, diagonal element and initialize pressure
#pragma  omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        ComputeSourceTerm(i);
        ComputeDiagonalElement(i);
        InitializePressure(i);
    }


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

void FluidSolver::IISPHFluidSolver::CalculateDensity(uint32_t particleIndex) {
    glm::vec2 position = ParticleCollection->GetPosition(particleIndex);

    float density = 0.0f;
    for (uint32_t neighbor: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
        auto type = ParticleCollection->GetParticleType(neighbor);
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighbor);
        float neighborMass = ParticleCollection->GetMass(neighbor);
        density += neighborMass * kernel->GetKernelValue(neighborPosition, position, KernelSupport);
    }

    ParticleCollection->SetDensity(particleIndex, density);
}

void FluidSolver::IISPHFluidSolver::CalculateNonPressureAccelerationAndPredictedVelocity(uint32_t particleIndex) {
    auto nonPressureAcc = ParticleCollection->GetNonPressureAcceleration(particleIndex);
    auto velocity = ParticleCollection->GetVelocity(particleIndex);
    // adding gravity to non pressure acceleration
    nonPressureAcc += glm::vec2(0.0f, -Gravity);

    // calculate predicted velocity
    glm::vec2 predictedVelocity = velocity + Timestep * nonPressureAcc;

    // set predicted velocity and reset non pressure accelerations
    ParticleCollection->SetPredictedVelocity(particleIndex, predictedVelocity);
    ParticleCollection->SetNonPressureAcceleration(particleIndex, glm::vec2(0.0f));
}

void FluidSolver::IISPHFluidSolver::ComputeSourceTerm(uint32_t particleIndex) {

}

void FluidSolver::IISPHFluidSolver::ComputeDiagonalElement(uint32_t particleIndex) {

}

void FluidSolver::IISPHFluidSolver::InitializePressure(uint32_t particleIndex) {

}
