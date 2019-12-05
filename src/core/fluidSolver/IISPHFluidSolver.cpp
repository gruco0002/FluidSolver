//
// Created by corbi on 20.11.2019.
//

#include <iostream>
#include "IISPHFluidSolver.hpp"


float FluidSolver::IISPHFluidSolver::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::IISPHFluidSolver::setParticleSize(float particleSize) {
    if (this->ParticleSize != particleSize || neighborhoodSearch == nullptr) {
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
    if (neighborhoodSearch != nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = new HashedNeighborhoodSearch(this->getParticleSize() * 3);
    }
    this->ParticleCollection = ParticleCollection;
    this->maxPredictedDensityErrorReached = 0.0f;
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

void FluidSolver::IISPHFluidSolver::ExecuteSimulationStep() {

    // find neighbors for all particles
    neighborhoodSearch->SetParticleCount(ParticleCollection->GetSize());
    neighborhoodSearch->FindNeighbors(ParticleCollection, NeighborhoodRadius);

    // calculating density and non pressure accelerations
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        auto type = ParticleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeDead)
            continue; // we do not want to process dead particles

        CalculateDensity(i);
        CalculateNonPressureAccelerationAndPredictedVelocity(i);
    }

    // compute source term, diagonal element and initialize pressure
#pragma  omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        auto type = ParticleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeDead)
            continue; // we do not want to process dead particles
        if (type == IParticleCollection::ParticleTypeBoundary)
            continue; // we do not want to process boundary particles

        ComputeSourceTerm(i);
        ComputeDiagonalElement(i);
        InitializePressure(i);
    }


    // compute pressure
    ComputePressure();


    // update velocity and position of all particles
#pragma omp parallel for
    for (int64_t i = 0; i < ParticleCollection->GetSize(); i++) {
        IntegrateParticle(i);
    }

}


void FluidSolver::IISPHFluidSolver::CalculateDensity(uint32_t particleIndex) {
    auto particleType = ParticleCollection->GetParticleType(particleIndex);
    if (particleType == IParticleCollection::ParticleTypeBoundary) {
        return; // don't calculate density for the boundary particles
    }
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
    if (type != IParticleCollection::ParticleTypeBoundary) {
        nonPressureAcc += glm::vec2(0.0f, -Gravity);
        nonPressureAcc += ComputeViscosityAcceleration(particleIndex);
    }

    // calculate predicted velocity
    glm::vec2 predictedVelocity = velocity + Timestep * nonPressureAcc;

    // set predicted velocity and reset non pressure accelerations
    ParticleCollection->SetPredictedVelocity(particleIndex, predictedVelocity);
    ParticleCollection->SetNonPressureAcceleration(particleIndex, glm::vec2(0.0f));
}

glm::vec2 FluidSolver::IISPHFluidSolver::ComputeViscosityAcceleration(uint32_t particleIndex) {
    glm::vec2 position = ParticleCollection->GetPosition(particleIndex);
    glm::vec2 velocity = ParticleCollection->GetVelocity(particleIndex);


    glm::vec2 tmp = glm::vec2(0.0f);
    for (uint32_t neighbor: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
        auto type = ParticleCollection->GetParticleType(neighbor);
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighbor);
        glm::vec2 neighborVelocity = ParticleCollection->GetVelocity(neighbor);
        float neighborMass = ParticleCollection->GetMass(neighbor);
        float neighborDensity = ParticleCollection->GetDensity(neighbor);

        if (neighborDensity == 0.0f)
            continue;

        glm::vec2 vij = velocity - neighborVelocity;
        glm::vec2 xij = position - neighborPosition;

        tmp += (neighborMass / neighborDensity) *
               (glm::dot(vij, xij) / (glm::dot(xij, xij) + 0.01f * ParticleSize * ParticleSize)) *
               kernel->GetKernelDerivativeReversedValue(neighborPosition, position, KernelSupport);


    }

    glm::vec2 viscosityAcceleration = 2.0f * Viscosity * tmp;
    return viscosityAcceleration;
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
    float sum = 0.0f;
    glm::vec2 particlePosition = ParticleCollection->GetPosition(particleIndex);
    float particleMass = ParticleCollection->GetMass(particleIndex);

    // first and third part of the sum (since we sum over normal particles and boundary particles)
    for (uint32_t neighborIndex: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
        auto neighborType = ParticleCollection->GetParticleType(neighborIndex);
        if (neighborType == IParticleCollection::ParticleTypeDead)
            continue; // we do not want to process dead particles

        float neighborMass = ParticleCollection->GetMass(neighborIndex);
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighborIndex);

        glm::vec2 internalSum = glm::vec2(0.0f);
        for (uint32_t internalNeighbor: neighborhoodSearch->GetParticleNeighbors(
                particleIndex)) {
            auto internalNeighborType = ParticleCollection->GetParticleType(internalNeighbor);
            if (internalNeighborType == IParticleCollection::ParticleTypeDead)
                continue; // we do not want to process dead particles

            float internalNeighborMass = ParticleCollection->GetMass(internalNeighbor);
            glm::vec2 internalNeighborPosition = ParticleCollection->GetPosition(internalNeighbor);

            if (internalNeighborType == IParticleCollection::ParticleTypeNormal) {
                internalSum += internalNeighborMass / RestDensity / RestDensity *
                               kernel->GetKernelDerivativeReversedValue(internalNeighborPosition, particlePosition,
                                                                        KernelSupport);
            } else if (internalNeighborType == IParticleCollection::ParticleTypeBoundary) {
                internalSum += 2.0f * Gamma * internalNeighborMass / RestDensity / RestDensity *
                               kernel->GetKernelDerivativeReversedValue(internalNeighborPosition, particlePosition,
                                                                        KernelSupport);
            }

            // since we have no factor for boundary particles, they can be handled as normal particles

        }

        sum += neighborMass *
               glm::dot(-internalSum, kernel->GetKernelDerivativeReversedValue(neighborPosition, particlePosition,
                                                                               KernelSupport));


    }

    // second part of the sum
    for (uint32_t neighborIndex: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
        auto neighborType = ParticleCollection->GetParticleType(neighborIndex);
        if (neighborType == IParticleCollection::ParticleTypeDead)
            continue; // we do not want to process dead particles

        if (neighborType != IParticleCollection::ParticleTypeNormal)
            continue; // this loop only processes normal particles

        float neighborMass = ParticleCollection->GetMass(neighborIndex);
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighborIndex);

        sum += neighborMass * glm::dot(particleMass / RestDensity / RestDensity *
                                       kernel->GetKernelDerivativeReversedValue(particlePosition, neighborPosition,
                                                                                KernelSupport),
                                       kernel->GetKernelDerivativeReversedValue(neighborPosition, particlePosition,
                                                                                KernelSupport));

    }


    float diagonalElement = Timestep * Timestep * sum;
    ParticleCollection->SetDiagonalElement(particleIndex, diagonalElement);
}

void FluidSolver::IISPHFluidSolver::InitializePressure(uint32_t particleIndex) {
    ParticleCollection->SetPressure(particleIndex, 0.0f);
}

void FluidSolver::IISPHFluidSolver::IntegrateParticle(uint32_t particleIndex) {
    auto type = ParticleCollection->GetParticleType(particleIndex);
    if (type == IParticleCollection::ParticleTypeBoundary) {
        return; // don't calculate unnecessary values for the boundary particles.
    }
    if (type == IParticleCollection::ParticleTypeDead) {
        return; // don't calculate unnecessary values for dead particles.
    }

    // integrate using euler cromer
    glm::vec2 acceleration = ParticleCollection->GetAcceleration(particleIndex);
    glm::vec2 velocity = ParticleCollection->GetPredictedVelocity(particleIndex) + Timestep * acceleration;
    glm::vec2 position = ParticleCollection->GetPosition(particleIndex) + Timestep * velocity;

    ParticleCollection->SetVelocity(particleIndex, velocity);
    ParticleCollection->SetPosition(particleIndex, position);
}

void FluidSolver::IISPHFluidSolver::ComputePressure() {

    size_t iteration = 0;
    float predictedDensityError = 0.0f;

    // iteration
    while ((iteration < MinNumberOfIterations || abs(predictedDensityError) > MaxDensityErrorAllowed) &&
           iteration < MaxNumberOfIterations) {

        // pre calculations
        predictedDensityError = 0.0f;
        size_t densityErrorCounter = 0;

        // first loop: compute pressure acceleration
#pragma omp parallel for
        for (int64_t particleIndex = 0; particleIndex < ParticleCollection->GetSize(); particleIndex++) {
            auto particleType = ParticleCollection->GetParticleType(particleIndex);
            if (particleType == IParticleCollection::ParticleTypeDead)
                continue; // we do not want to process dead particles
            if (particleType == IParticleCollection::ParticleTypeBoundary)
                continue;

            float particlePressure = ParticleCollection->GetPressure(particleIndex);
            glm::vec2 particlePosition = ParticleCollection->GetPosition(particleIndex);

            glm::vec2 sum = glm::vec2(0.0f);
            for (uint32_t neighborIndex: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
                auto neighborType = ParticleCollection->GetParticleType(neighborIndex);
                if (neighborType == IParticleCollection::ParticleTypeDead)
                    continue; // we do not want to process dead particles

                glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighborIndex);
                float neighborMass = ParticleCollection->GetMass(neighborIndex);

                if (neighborType == IParticleCollection::ParticleTypeNormal) {
                    float neighborPressure = ParticleCollection->GetPressure(neighborIndex);

                    sum += neighborMass * (particlePressure / RestDensity / RestDensity +
                                           neighborPressure / RestDensity / RestDensity) *
                           kernel->GetKernelDerivativeReversedValue(neighborPosition, particlePosition,
                                                                    KernelSupport);
                } else if (neighborType == IParticleCollection::ParticleTypeBoundary) {
                    // pressure is mirrored here
                    sum += Gamma * neighborMass * 2.0f * (particlePressure / RestDensity / RestDensity) *
                           kernel->GetKernelDerivativeReversedValue(neighborPosition, particlePosition,
                                                                    KernelSupport);
                }

            }
            ParticleCollection->SetAcceleration(particleIndex, -sum);
        }

        // second loop
#pragma omp parallel for
        for (int64_t particleIndex = 0; particleIndex < ParticleCollection->GetSize(); particleIndex++) {
            auto particleType = ParticleCollection->GetParticleType(particleIndex);
            if (particleType == IParticleCollection::ParticleTypeDead)
                continue; // we do not want to process dead particles
            if (particleType == IParticleCollection::ParticleTypeBoundary)
                continue;

            // First step calculate Ap
            glm::vec2 particlePressureAcceleration = ParticleCollection->GetAcceleration(particleIndex);
            glm::vec2 particlePosition = ParticleCollection->GetPosition(particleIndex);

            float sum = 0.0f;
            for (uint32_t neighborIndex: neighborhoodSearch->GetParticleNeighbors(particleIndex)) {
                auto neighborType = ParticleCollection->GetParticleType(neighborIndex);
                if (neighborType == IParticleCollection::ParticleTypeDead)
                    continue; // we do not want to process dead particles

                float neighborMass = ParticleCollection->GetMass(neighborIndex);
                glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighborIndex);

                if (neighborType == IParticleCollection::ParticleTypeNormal) {
                    glm::vec2 neighborPressureAcceleration = ParticleCollection->GetAcceleration(neighborIndex);

                    sum += neighborMass * glm::dot((particlePressureAcceleration - neighborPressureAcceleration),
                                                   kernel->GetKernelDerivativeReversedValue(neighborPosition,
                                                                                            particlePosition,
                                                                                            KernelSupport));

                } else if (neighborType == IParticleCollection::ParticleTypeBoundary) {
                    sum += neighborMass * glm::dot(particlePressureAcceleration,
                                                   kernel->GetKernelDerivativeReversedValue(neighborPosition,
                                                                                            particlePosition,
                                                                                            KernelSupport));
                }
            }

            float Ap = Timestep * Timestep * sum;

            // Second step: Update pressure
            float particleDiagonalElement = ParticleCollection->GetDiagonalElement(particleIndex);
            float particleSourceTerm = ParticleCollection->GetSourceTerm(particleIndex);
            if (std::abs(particleDiagonalElement) > std::numeric_limits<float>::epsilon()) {
                float particlePressure = ParticleCollection->GetPressure(particleIndex);
                float particlePressureNextStep = std::fmax(0.0f, particlePressure + Omega * ((particleSourceTerm - Ap) /
                                                                                             particleDiagonalElement));
                // update pressure
                ParticleCollection->SetPressure(particleIndex, particlePressureNextStep);

            }

            // Info: The calculation of the particle density error is based on the implementation of IISPH by Stefan Band
            // Third step: Calculate predicted density error
            float particleDensityError = std::abs(particleSourceTerm - Ap);
            if (std::abs(particleDiagonalElement) > std::numeric_limits<float>::epsilon()) {
                if (std::abs(ParticleCollection->GetPressure(particleIndex)) <= std::numeric_limits<float>::epsilon()) {
                    // if the new pressure is zero, we do not have a density error
                    predictedDensityError = 0.0f;
                }

                predictedDensityError += particleDensityError;
                densityErrorCounter++;
            }


        }

        if (densityErrorCounter != 0) {
            // post calculations: calculate arithmetic average density error
            predictedDensityError = predictedDensityError / (float) densityErrorCounter;
            //std::cout << iteration << "\t" << predictedDensityError << std::endl;
        } else {
            predictedDensityError = 0.0f;
            //std::cout << iteration << "\t" << "No particles contributing to density error" << std::endl;
        }

        // increate iteration count
        iteration++;

    }

    lastPredictedDensityError = predictedDensityError;
    maxPredictedDensityErrorReached = std::max(maxPredictedDensityErrorReached, lastPredictedDensityError);
    lastIterationCount = iteration;


}

float FluidSolver::IISPHFluidSolver::getLastPredictedDensityError() const {
    return lastPredictedDensityError;
}

size_t FluidSolver::IISPHFluidSolver::getLastIterationCount() const {
    return lastIterationCount;
}

float FluidSolver::IISPHFluidSolver::getMaxPredictedDensityErrorReached() const {
    return maxPredictedDensityErrorReached;
}

