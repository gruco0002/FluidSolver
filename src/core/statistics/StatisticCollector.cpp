#include <algorithm>
#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
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
    iterationCount->Set(0u);
    predictedDensityError->Set(0.0f);
    calculatedAverageDensityError->Set(0.0f);

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

        if (type != ParticleTypeDead) {
            // potential energy
            *calculatedPotentialEnergy += (position.y - zeroHeight) * mass * Gravity;

            // kinetic energy
            *calculatedKineticEnergy += 0.5f * mass * particleVelocity * particleVelocity;

            // velocity
            calculatedMaximumVelocity->Max(glm::length(velocity));
        }

        if (type == ParticleTypeNormal) {
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
        if (type == ParticleTypeNormal) {
            *calculatedNormalParticleCount += 1u;
        } else if (type == ParticleTypeBoundary) {
            *calculatedBoundaryParticleCount += 1u;
        } else if (type == ParticleTypeDead) {
            *calculatedDeadParticleCount += 1u;
        }

    }

    calculatedCFLNumber->Set(calculatedMaximumVelocity->FloatValue * Timestep / ParticleSize);
    calculatedEnergy->Set(calculatedKineticEnergy->FloatValue + calculatedPotentialEnergy->FloatValue);

    if (calculatedAverageDensityCounter > 0) {
        *calculatedAverageDensity /= (float) calculatedAverageDensityCounter;
        calculatedAverageDensityError->Set(calculatedAverageDensity->FloatValue - RestDensity);
    }

    if (diagonalCounter > 0)
        *diagonalElement /= (float) diagonalCounter;

    auto iisphSolver = dynamic_cast<IISPHFluidSolver *>(fluidSolver);
    if (iisphSolver != nullptr) {
        iterationCount->Set((uint32_t) iisphSolver->getLastIterationCount());
        predictedDensityError->Set(iisphSolver->getLastPredictedDensityError());
    }

    pressureSolverComputationTimePerTimestep->Set(
            fluidSolver->GetComputationTimePressureSolverLastTimestepInMicroseconds());
    computationTimePerTimestep->Set(fluidSolver->GetComputationTimeLastTimestepInMicroseconds());

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
    iterationCount = new StatValue("Iteration Count",
                                   "Number of iterations the IISPH Fluid Solver needed to calculate this timestep.",
                                   StatValue::StatValueTypeUInt);
    predictedDensityError = new StatValue("Predicted Density Error",
                                          "The last predicted density error of the IISPH Fluid Solver.",
                                          StatValue::StatValueTypeFloat);
    calculatedAverageDensityError = new StatValue("Average Density Error",
                                                  "Average Density Error is only considered for particles of normal type whose density is larger or equal to the rest density. The error is calculated by substracting the Rest Density from the Average Density.",
                                                  StatValue::StatValueTypeFloat);
    computationTimePerTimestep = new StatValue("Computation Time",
                                               "Time in microseconds needed to compute one timestep of the simulation.",
                                               StatValue::StatValueTypeUInt);
    pressureSolverComputationTimePerTimestep = new StatValue("Pressure Solver Time",
                                                             "Time in microseconds needed to execute the pressure solver in a timestep.",
                                                             StatValue::StatValueTypeUInt);

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
    Stats.push_back(iterationCount);
    Stats.push_back(predictedDensityError);
    Stats.push_back(calculatedAverageDensityError);
    Stats.push_back(computationTimePerTimestep);
    Stats.push_back(pressureSolverComputationTimePerTimestep);
}

void StatisticCollector::CleanUpFields() {
    for (StatValue *tmp: Stats) {
        delete tmp;
    }
}

FluidSolver::IFluidSolver *StatisticCollector::getFluidSolver() const {
    return fluidSolver;
}

void StatisticCollector::setFluidSolver(FluidSolver::IFluidSolver *fluidSolver) {
    StatisticCollector::fluidSolver = fluidSolver;
}
