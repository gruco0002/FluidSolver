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
