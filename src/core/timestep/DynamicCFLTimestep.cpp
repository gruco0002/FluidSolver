//
// Created by Corbinian Gruber on 12.12.19.
//

#include <algorithm>
#include "DynamicCFLTimestep.hpp"

void FluidSolver::DynamicCFLTimestep::CalculateCurrentTimestep() {
    float maxVelocity = CalculateMaximumVelocity();
    float timestep = MinimumTimestep;
    if (maxVelocity > std::numeric_limits<float>::epsilon()) {
        // try to obtain cfl number
        timestep = CFLNumber * ParticleSize / maxVelocity;
        timestep = std::min(MaximumTimestep, std::max(MinimumTimestep, timestep));
    }
    currentTimestep = timestep;
}

float FluidSolver::DynamicCFLTimestep::getCurrentTimestep() {
    return currentTimestep;
}

float FluidSolver::DynamicCFLTimestep::CalculateMaximumVelocity() {
    float maximum = 0.0f;

    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto type = particleCollection->GetParticleType(i);
        if (type == ParticleTypeDead)
            continue;
        auto velocity = particleCollection->GetVelocity(i);
        maximum = std::max(maximum, (float)glm::length(velocity));
    }
    return maximum;
}

float FluidSolver::DynamicCFLTimestep::getMaximumTimestep() const {
    return MaximumTimestep;
}

void FluidSolver::DynamicCFLTimestep::setMaximumTimestep(float maximumTimestep) {
    MaximumTimestep = maximumTimestep;
}

float FluidSolver::DynamicCFLTimestep::getMinimumTimestep() const {
    return MinimumTimestep;
}

void FluidSolver::DynamicCFLTimestep::setMinimumTimestep(float minimumTimestep) {
    MinimumTimestep = minimumTimestep;
}

float FluidSolver::DynamicCFLTimestep::getCflNumber() const {
    return CFLNumber;
}

void FluidSolver::DynamicCFLTimestep::setCflNumber(float cflNumber) {
    CFLNumber = cflNumber;
}

FluidSolver::DynamicCFLTimestep::DynamicCFLTimestep(float maximumTimestep, float minimumTimestep, float cflNumber)
        : MaximumTimestep(maximumTimestep), MinimumTimestep(minimumTimestep), CFLNumber(cflNumber) {}

FluidSolver::DynamicCFLTimestep::DynamicCFLTimestep() {}
