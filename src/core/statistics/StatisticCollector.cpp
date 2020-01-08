//
// Created by corbi on 12.06.2019.
//

#include <algorithm>
#include "StatisticCollector.hpp"

using namespace FluidSolver;


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

void StatisticCollector::CalculateData() {

    calculatedPotentialEnergy = 0.0f;
    calculatedKineticEnergy = 0.0f;
    calculatedAverageDensity = 0.0f;
    calculatedMaximumVelocity = 0.0f;
    calculatedAverageDensity = 0.0f;
    uint32_t calculatedAverageDensityCounter = 0;
    calculatedNormalParticleCount = 0;
    calculatedBoundaryParticleCount = 0;
    calculatedDeadParticleCount = 0;

    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        if (!particleSelection->IsParticleSelected(i, particleCollection)) {
            continue;
        }

        auto type = particleCollection->GetParticleType(i);
        auto velocity = particleCollection->GetVelocity(i);
        auto position = particleCollection->GetPosition(i);
        auto mass = particleCollection->GetMass(i);
        float particleVelocity = glm::length(velocity);
        auto density = particleCollection->GetDensity(i);

        if (type != IParticleCollection::ParticleTypeDead) {
            // potential energy
            calculatedPotentialEnergy += (position.y - zeroHeight) * mass * Gravity;

            // kinetic energy
            calculatedKineticEnergy += 0.5f * mass * particleVelocity * particleVelocity;

            // velocity
            calculatedMaximumVelocity = std::max(calculatedMaximumVelocity, glm::length(velocity));
        }

        if (type == IParticleCollection::ParticleTypeNormal) {
            // density
            if (density >= RestDensity) {
                calculatedAverageDensity += density;
                calculatedAverageDensityCounter++;
            }
        }

        // counter
        if (type == IParticleCollection::ParticleTypeNormal) {
            calculatedNormalParticleCount++;
        } else if (type == IParticleCollection::ParticleTypeBoundary) {
            calculatedBoundaryParticleCount++;
        } else if (type == IParticleCollection::ParticleTypeDead) {
            calculatedDeadParticleCount++;
        }

    }

    calculatedCFLNumber = calculatedMaximumVelocity * Timestep / ParticleSize;
    calculatedEnergy = calculatedKineticEnergy + calculatedPotentialEnergy;

    if (calculatedAverageDensityCounter > 0)
        calculatedAverageDensity = calculatedAverageDensity / (float) calculatedAverageDensityCounter;

}

float StatisticCollector::getCalculatedAverageDensity() const {
    return calculatedAverageDensity;
}

float StatisticCollector::getCalculatedEnergy() const {
    return calculatedEnergy;
}

float StatisticCollector::getCalculatedMaximumVelocity() const {
    return calculatedMaximumVelocity;
}

uint32_t StatisticCollector::getCalculatedDeadParticleCount() const {
    return calculatedDeadParticleCount;
}

float StatisticCollector::getCalculatedKineticEnergy() const {
    return calculatedKineticEnergy;
}

float StatisticCollector::getCalculatedPotentialEnergy() const {
    return calculatedPotentialEnergy;
}

uint32_t StatisticCollector::getCalculatedBoundaryParticleCount() const {
    return calculatedBoundaryParticleCount;
}

uint32_t StatisticCollector::getCalculatedNormalParticleCount() const {
    return calculatedNormalParticleCount;
}

float StatisticCollector::getCalculatedCflNumber() const {
    return calculatedCFLNumber;
}

StatisticCollector::~StatisticCollector() {
    delete particleSelection;
}

IParticleSelection *StatisticCollector::getParticleSelection() const {
    return particleSelection;
}

void StatisticCollector::setParticleSelection(IParticleSelection *particleSelection) {
    StatisticCollector::particleSelection = particleSelection;
}
