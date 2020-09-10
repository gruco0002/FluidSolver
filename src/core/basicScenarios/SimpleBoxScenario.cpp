#include "SimpleBoxScenario.hpp"

void
FluidSolver::SimpleBoxScenario::ResetData(FluidSolver::ParticleCollection *collection, float restDensity) {

    collection->clear();

    float mass = restDensity * GetParticleSize() * GetParticleSize();


    // generate a simple boundary

    // bottom boundary
    for (int x = -23; x <= 12; x++) {
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) x, (float) -14);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) x, (float) -15);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) x, (float) -16);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
    }

    // left boundary
    for (int y = 20; y >= -13; y--) {
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) -21, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) -22, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) -23, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
    }

    // right boundary
    for (int y = 20; y >= -13; y--) {
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) 11, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) 12, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }
        {
            auto id = collection->add();
            auto &mData = collection->get<MovementData>(id);
            auto &pData = collection->get<ParticleData>(id);
            auto &iData = collection->get<ParticleInfo>(id);

            mData.position = glm::vec2((float) 10, (float) y);
            mData.velocity = glm::vec2(0.0f);
            mData.acceleration = glm::vec2(0.0f);
            pData.pressure = 0.0f;
            pData.density = restDensity;
            pData.mass = mass;
            iData.type = FluidSolver::ParticleTypeBoundary;
            iData.index = id;
        }


    }

    // particles
    for (int x = -20; x <= 0; x++) {
        for (int y = 7; y >= -13; y--) {
            // normal particle
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
            iData.type = FluidSolver::ParticleTypeNormal;
            iData.index = id;
        }
    }


}

int FluidSolver::SimpleBoxScenario::GetParticleCountX() {
    return 45;
}

int FluidSolver::SimpleBoxScenario::GetParticleCountY() {
    return 60;
}

/*std::vector<FluidSolver::ISimulationModifier *> FluidSolver::SimpleBoxScenario::GetSimulationModifiers() {
    return {new DeathBox(25, -25, -20, 15)};
}*/

std::string FluidSolver::SimpleBoxScenario::GetName() {
    return "Simple box";
}
