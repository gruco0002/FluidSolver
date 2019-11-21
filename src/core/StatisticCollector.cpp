//
// Created by corbi on 12.06.2019.
//

#include <algorithm>
#include "StatisticCollector.hpp"

using namespace FluidSolver;

float StatisticCollector::CalculateAverageDensity() {

    double densitySum = 0;
    uint32_t sumCount = 0;

    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type != IParticleCollection::ParticleTypeNormal)
            continue;
        auto density = particleCollection->GetDensity(i);
        if (density < RestDensity)
            continue;
        densitySum += density;
        sumCount++;
    }

    return densitySum / (double) sumCount;
}

float StatisticCollector::CalculateEnergy() {
    float potentialEnergySum = 0.0f;
    float kineticEnergySum = 0.0f;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type != IParticleCollection::ParticleTypeNormal)
            continue;

        auto velocity = particleCollection->GetVelocity(i);
        auto position = particleCollection->GetPosition(i);
        auto mass = particleCollection->GetMass(i);
        float particleVelocity = glm::length(velocity);

        // potential energy
        potentialEnergySum += (position.y - zeroHeight) * mass * Gravity;

        // kinetic energy
        kineticEnergySum += 0.5f * mass * particleVelocity * particleVelocity;

    }

    return CalculateEnergy(kineticEnergySum, potentialEnergySum);
}

float StatisticCollector::CalculateMaximumVelocity() {
    if (particleCollection->GetSize() == 0)return 0;
    float maximum = glm::length(particleCollection->GetVelocity(0));

    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type != IParticleCollection::ParticleTypeNormal)
            continue;
        auto velocity = particleCollection->GetVelocity(i);
        maximum = std::max(maximum, glm::length(velocity));
    }
    return maximum;
}

StatisticCollector::StatisticCollector() {}

float StatisticCollector::CalculateEnergy(float kineticEnergy, float potentialEnergy) {
    return kineticEnergy + potentialEnergy;
}

uint32_t StatisticCollector::GetDeadParticleCount() {
    uint32_t counter = 0;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeDead)
            counter++;
    }
    return counter;
}

float StatisticCollector::CalculateKineticEnergy() {
    float kineticEnergySum = 0.0f;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type != IParticleCollection::ParticleTypeNormal)
            continue;

        auto velocity = particleCollection->GetVelocity(i);
        auto mass = particleCollection->GetMass(i);
        float particleVelocity = glm::length(velocity);

        // kinetic energy
        kineticEnergySum += 0.5f * mass * particleVelocity * particleVelocity;

    }
    return kineticEnergySum;
}

float StatisticCollector::CalculatePotentialEnergy() {
    float potentialEnergySum = 0.0f;

    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type != IParticleCollection::ParticleTypeNormal)
            continue;


        auto position = particleCollection->GetPosition(i);
        auto mass = particleCollection->GetMass(i);

        // potential energy
        potentialEnergySum += (position.y - zeroHeight) * mass * Gravity;


    }
    return potentialEnergySum;
}

uint32_t StatisticCollector::GetBoundaryParticleCount() {
    uint32_t counter = 0;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeBoundary)
            counter++;
    }
    return counter;
}

float StatisticCollector::GetCFLNumber() {
    float maximumVelocity = CalculateMaximumVelocity();
    return GetCFLNumber(maximumVelocity);
}

float StatisticCollector::GetCFLNumber(float maximumVelocity) {
    float cfl = maximumVelocity * Timestep / ParticleSize;
    return cfl;
}

uint32_t StatisticCollector::GetNormalParticleCount() {
    uint32_t counter = 0;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type == IParticleCollection::ParticleTypeNormal)
            counter++;
    }
    return counter;
}

IParticleCollection *StatisticCollector::getParticleCollection() const {
    return particleCollection;
}

void StatisticCollector::setParticleCollection(IParticleCollection *particleCollection) {
    StatisticCollector::particleCollection = particleCollection;
}

float StatisticCollector::getTimestep() const {
    return Timestep;
}

void StatisticCollector::setTimestep(float timestep) {
    Timestep = timestep;
}

float StatisticCollector::getParticleSize() const {
    return ParticleSize;
}

void StatisticCollector::setParticleSize(float particleSize) {
    ParticleSize = particleSize;
}

float StatisticCollector::getGravity() const {
    return Gravity;
}

void StatisticCollector::setGravity(float gravity) {
    Gravity = gravity;
}

float StatisticCollector::getRestDensity() const {
    return RestDensity;
}

void StatisticCollector::setRestDensity(float restDensity) {
    RestDensity = restDensity;
}
