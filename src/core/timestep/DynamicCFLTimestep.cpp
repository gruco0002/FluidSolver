//
// Created by Corbinian Gruber on 12.12.19.
//

#include <algorithm>
#include "DynamicCFLTimestep.hpp"

void FluidSolver::DynamicCFLTimestep::CalculateCurrentTimestep() {
    float maxVelocity = CalculateMaximumVelocity();
    float timestep = minimumTimestep;
    if (maxVelocity > std::numeric_limits<float>::epsilon()) {
        // try to obtain cfl number
        timestep = CFLNumber * ParticleSize / maxVelocity;
        timestep = std::min(maximumTimestep, std::max(minimumTimestep, timestep));
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
