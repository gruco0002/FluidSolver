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
    this->Gravity = gravity;
}

float StatisticCollector::getRestDensity() const {
    return RestDensity;
}

void StatisticCollector::setRestDensity(float restDensity) {
    RestDensity = restDensity;
}

void StatisticCollector::CalculateData() {

    calculatedPotentialEnergy->Set(0.0f);
    calculatedKineticEnergy->Set(0.0f);
    calculatedAverageDensity->Set(0.0f);
    calculatedMaximumVelocity->Set(0.0f);
    calculatedAverageDensity->Set(0.0f);
    uint32_t calculatedAverageDensityCounter = 0;
    calculatedNormalParticleCount->Set(0u);
    calculatedBoundaryParticleCount->Set(0u);
    calculatedDeadParticleCount->Set(0u);
    diagonalElement->Set(0.0f);
    uint32_t diagonalCounter = 0;

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
        auto diagonal = particleCollection->GetDiagonalElement(i);

        if (type != IParticleCollection::ParticleTypeDead) {
            // potential energy
            *calculatedPotentialEnergy += (position.y - zeroHeight) * mass * Gravity;

            // kinetic energy
            *calculatedKineticEnergy += 0.5f * mass * particleVelocity * particleVelocity;

            // velocity
            calculatedMaximumVelocity->Max(glm::length(velocity));
        }

        if (type == IParticleCollection::ParticleTypeNormal) {
            // density
            if (density >= RestDensity) {
                *calculatedAverageDensity += density;
                calculatedAverageDensityCounter++;
            }

            // diagonal element
            *diagonalElement += diagonal;
            diagonalCounter++;
        }

        // counter
        if (type == IParticleCollection::ParticleTypeNormal) {
            *calculatedNormalParticleCount += 1u;
        } else if (type == IParticleCollection::ParticleTypeBoundary) {
            *calculatedBoundaryParticleCount += 1u;
        } else if (type == IParticleCollection::ParticleTypeDead) {
            *calculatedDeadParticleCount += 1u;
        }

    }

    calculatedCFLNumber->Set(calculatedMaximumVelocity->FloatValue * Timestep / ParticleSize);
    calculatedEnergy->Set(calculatedKineticEnergy->FloatValue + calculatedPotentialEnergy->FloatValue);

    if (calculatedAverageDensityCounter > 0)
        *calculatedAverageDensity /= (float) calculatedAverageDensityCounter;

    if (diagonalCounter > 0)
        *diagonalElement /= (float) diagonalCounter;

}


StatisticCollector::~StatisticCollector() {
    CleanUpFields();
}

IParticleSelection *StatisticCollector::getParticleSelection() const {
    return particleSelection;
}

void StatisticCollector::setParticleSelection(IParticleSelection *particleSelection) {
    this->particleSelection = particleSelection;
}

void StatisticCollector::SetupFields() {
    CleanUpFields();

    calculatedAverageDensity = new StatValue("Average Density",
                                             "Average Density is only considered for particles of normal type whose density is larger or equal to the rest density.",
                                             StatValue::StatValueTypeFloat);
    calculatedEnergy = new StatValue("Energy", "", StatValue::StatValueTypeFloat);
    calculatedMaximumVelocity = new StatValue("Max Velocity", "", StatValue::StatValueTypeFloat);
    calculatedDeadParticleCount = new StatValue("Dead Particles", "", StatValue::StatValueTypeUInt);
    calculatedKineticEnergy = new StatValue("Kinetic Energy", "", StatValue::StatValueTypeFloat);
    calculatedPotentialEnergy = new StatValue("Potential Energy", "", StatValue::StatValueTypeFloat);
    calculatedBoundaryParticleCount = new StatValue("Boundary Particles", "", StatValue::StatValueTypeUInt);
    calculatedNormalParticleCount = new StatValue("Normal Particles", "", StatValue::StatValueTypeUInt);
    calculatedCFLNumber = new StatValue("CFL Number", "", StatValue::StatValueTypeFloat);
    diagonalElement = new StatValue("Diagonal Element", "Average diagonal element value of all normal particles.",
                                    StatValue::StatValueTypeFloat);

    RefreshFieldVector();
}

StatisticCollector::StatisticCollector() {
    SetupFields();
}

const std::vector<StatValue *> &StatisticCollector::getStats() const {
    return Stats;
}

void StatisticCollector::RefreshFieldVector() {
    Stats.clear();
    Stats.push_back(calculatedAverageDensity);
    Stats.push_back(calculatedEnergy);
    Stats.push_back(calculatedMaximumVelocity);
    Stats.push_back(calculatedDeadParticleCount);
    Stats.push_back(calculatedKineticEnergy);
    Stats.push_back(calculatedPotentialEnergy);
    Stats.push_back(calculatedBoundaryParticleCount);
    Stats.push_back(calculatedNormalParticleCount);
    Stats.push_back(calculatedCFLNumber);
    Stats.push_back(diagonalElement);
}

void StatisticCollector::CleanUpFields() {
    delete calculatedAverageDensity;
    delete calculatedEnergy;
    delete calculatedMaximumVelocity;
    delete calculatedDeadParticleCount;
    delete calculatedKineticEnergy;
    delete calculatedPotentialEnergy;
    delete calculatedBoundaryParticleCount;
    delete calculatedNormalParticleCount;
    delete calculatedCFLNumber;
    delete diagonalElement;
}
