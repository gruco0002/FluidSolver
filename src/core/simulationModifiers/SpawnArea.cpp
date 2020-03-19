//
// Created by corbi on 06.06.2019.
//

#include "SpawnArea.hpp"

void FluidSolver::SpawnArea::ModifySimulation(FluidSolver::IParticleCollection *collection, SimulationInfo &info) {

    if (spawnRate == 0.0f)
        return;

    float secondsPerParticle = 1.0f / spawnRate;

    timeAccumulator += info.timeStep;

    if (timeAccumulator < secondsPerParticle)
        return; // nothing to do for this time step

    // no parallelisation, since we do not know if the random generator supports it
    for (uint32_t i = 0; i < collection->GetSize(); i++) {
        auto type = collection->GetParticleType(i);
        if (type != ParticleTypeDead) {
            continue; // only dead particles should be processed
        }

        // check if we can spawn another particle
        if (timeAccumulator < secondsPerParticle)
            continue;
        timeAccumulator -= secondsPerParticle;

        // generate random position
        glm::vec2 position = GenerateRandomPosition();

        // check if we have a collision
        if (IsColliding(position, collection, info))
            continue;


        // update data, reset stuff
        collection->SetParticleType(i, ParticleTypeNormal);
        collection->SetPosition(i, position);
        collection->SetVelocity(i, initialVelocity);
        collection->SetAcceleration(i, glm::vec2(0.0f));
        collection->SetPressure(i, 0.0f);
        collection->SetDensity(i, info.restDensity);

    }
}

glm::vec2 FluidSolver::SpawnArea::GenerateRandomPosition() {

    unsigned int value1 = pseudoRandomGenerator() - std::minstd_rand::min();
    float normalizedValue1 = (float) value1 / (float) (std::minstd_rand::max() - std::minstd_rand::min());

    unsigned int value2 = pseudoRandomGenerator() - std::minstd_rand::min();
    float normalizedValue2 = (float) value2 / (float) (std::minstd_rand::max() - std::minstd_rand::min());

    float width = right - left;
    float height = top - bottom;

    return glm::vec2(left + width * normalizedValue1, bottom + height * normalizedValue2);
}

FluidSolver::SpawnArea::SpawnArea(float top, float left, float bottom, float right, float spawnRate,
                                  const glm::vec2 initialVelocity) : spawnRate(spawnRate), top(top), left(left),
                                                                     bottom(bottom), right(right),
                                                                     initialVelocity(initialVelocity) {}

bool FluidSolver::SpawnArea::IsColliding(glm::vec2 position, IParticleCollection *collection, SimulationInfo &info) {

    bool hasCollision = false;
    #pragma omp parallel for
    for (int64_t  i = 0; i < collection->GetSize(); i++) {
        auto type = collection->GetParticleType(i);
        if (type == ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        glm::vec2 partPos = collection->GetPosition(i);
        float distance = glm::length(partPos - position);
        if (distance <= info.particleSize) {
            hasCollision = true;
        }


    }

    return hasCollision;
}
