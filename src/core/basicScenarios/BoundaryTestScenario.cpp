#include "BoundaryTestScenario.hpp"

void
FluidSolver::BoundaryTestScenario::ResetData(FluidSolver::ParticleCollection *collection, float restDensity) {
    collection->clear();

    float mass = restDensity * GetParticleSize() * GetParticleSize();

    // generate a simple boundary
    for (int y = -5; y >= -7; y--) {
        for (int x = -10; x <= 10; x++) {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);


            mData.position = glm::vec2((float) x, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
    }

    // normal particle
    auto id = collection->add();
    auto &mData = collection->get<MovementData>(id);
    auto &pData = collection->get<ParticleData>(id);
    auto &iData = collection->get<ParticleInfo>(id);
    mData.position = glm::vec2(0.0f);
    mData.velocity = glm::vec2(0.0f);
    mData.acceleration = glm::vec2(0.0f);
    pData.pressure = 0.0f;
    pData.density = restDensity;
    pData.mass = mass;
    iData.type = FluidSolver::ParticleTypeNormal;
    iData.index = id;

}

int FluidSolver::BoundaryTestScenario::GetParticleCountX() {
    return 20;
}

int FluidSolver::BoundaryTestScenario::GetParticleCountY() {
    return 20;
}

std::string FluidSolver::BoundaryTestScenario::GetName() {
    return "Boundary Test";
}
