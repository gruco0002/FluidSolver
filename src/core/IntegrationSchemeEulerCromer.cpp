//
// Created by corbi on 24.04.2019.
//

#include "IntegrationSchemeEulerCromer.hpp"

void FluidSolver::IntegrationSchemeEulerCromer::Integrate(uint32_t particleIndex,
                                                          FluidSolver::IParticleCollection *particleCollection,
                                                          float timeStep) {

    glm::vec2 acceleration = particleCollection->GetAcceleration(particleIndex);
    glm::vec2 velocity = particleCollection->GetVelocity(particleIndex) + timeStep * acceleration;
    glm::vec2 position = particleCollection->GetPosition(particleIndex) + timeStep * velocity;

    particleCollection->SetVelocity(particleIndex, velocity);
    particleCollection->SetPosition(particleIndex, position);


}
