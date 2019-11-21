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
        // TODO: prevent dead particles
        CalculateDensity(i);
        CalculateNonPressureAccelerationAndPredictedVelocity(i);
    }

    // compute source term, diagonal element and initialize pressure
#pragma  omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        // TODO: prevent dead particles
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
    auto type = ParticleCollection->GetParticleType(particleIndex);
    auto nonPressureAcc = ParticleCollection->GetNonPressureAcceleration(particleIndex);
    auto velocity = ParticleCollection->GetVelocity(particleIndex);

    // adding gravity to non pressure acceleration
    if (type != IParticleCollection::ParticleTypeBoundary)
        nonPressureAcc += glm::vec2(0.0f, -Gravity);

    // calculate predicted velocity
    glm::vec2 predictedVelocity = velocity + Timestep * nonPressureAcc;

    // set predicted velocity and reset non pressure accelerations
    ParticleCollection->SetPredictedVelocity(particleIndex, predictedVelocity);
    ParticleCollection->SetNonPressureAcceleration(particleIndex, glm::vec2(0.0f));
}

void FluidSolver::IISPHFluidSolver::ComputeSourceTerm(uint32_t particleIndex) {
    float particleDensity = ParticleCollection->GetDensity(particleIndex);
    glm::vec2 particlePredictedVelocity = ParticleCollection->GetPredictedVelocity(particleIndex);
    glm::vec2 particlePosition = ParticleCollection->GetPosition(particleIndex);

    float sum = 0.0f;
    for (uint32_t neighborIndex: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
        auto neighborType = ParticleCollection->GetParticleType(neighborIndex);
        if (neighborType == IParticleCollection::ParticleTypeDead)
            continue; // we do not want to process dead particles

        float neighborMass = ParticleCollection->GetMass(neighborIndex);
        glm::vec2 neighborPredictedVelocity = ParticleCollection->GetPredictedVelocity(neighborIndex);
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighborIndex);

        sum += neighborMass * glm::dot((particlePredictedVelocity - neighborPredictedVelocity),
                                       kernel->GetKernelDerivativeReversedValue(neighborPosition, particlePosition,
                                                                                KernelSupport));
        // the velocity of a boundary particle in the next timestep is here equal to the predicted velocity of said
        // boundary particle, since there are no other forces that apply to them.

    }

    float sourceTerm = RestDensity - particleDensity - Timestep * sum;

    ParticleCollection->SetSourceTerm(particleIndex, sourceTerm);

}

void FluidSolver::IISPHFluidSolver::ComputeDiagonalElement(uint32_t particleIndex) {

}

void FluidSolver::IISPHFluidSolver::InitializePressure(uint32_t particleIndex) {

}
