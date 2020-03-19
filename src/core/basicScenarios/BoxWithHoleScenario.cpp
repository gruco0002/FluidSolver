#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include <core/simulationModifiers/DeathBox.hpp>
#include <core/simulationModifiers/SpawnArea.hpp>
#include "BoxWithHoleScenario.hpp"

void
FluidSolver::BoxWithHoleScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {

    particleCollection->Clear();

    float mass = restDensity * GetParticleSize() * GetParticleSize();


    // generate a simple boundary
    std::vector<FluidSolver::FluidParticle> particles;

    for (int x = -23; x <= 12; x++) {

        if (x >= -1 && x <= 1) {
            // hole
            continue;
        }

        FluidSolver::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::ParticleTypeBoundary;

        p.Position = glm::vec2((float) x, (float) -14);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -15);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -16);
        particles.push_back(p);
    }

    // left boundary
    for (int y = 20; y >= -13; y--) {
        FluidSolver::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -21, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -22, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -23, (float) y);
        particles.push_back(p);
    }

    // right boundary
    for (int y = 20; y >= -13; y--) {
        FluidSolver::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::ParticleTypeBoundary;

        p.Position = glm::vec2((float) 10, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 11, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 12, (float) y);
        particles.push_back(p);

    }

    // particles
    for (int x = -20; x <= 0; x++) {
        for (int y = 7; y >= -13; y--) {
            // normal particle
            FluidSolver::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::ParticleTypeNormal;
            particles.push_back(p);
        }
    }

    particleCollection->AddParticles(particles);
}

int FluidSolver::BoxWithHoleScenario::GetParticleCountX() {
    return 45;
}

int FluidSolver::BoxWithHoleScenario::GetParticleCountY() {
    return 60;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::BoxWithHoleScenario::GetSimulationModifiers() {
    return {new FluidSolver::DeathBox(28, -24, -20, 13),
            new FluidSolver::SpawnArea(25, -18, 22, -10, 40.0f, glm::vec2(0.0f, -6.0f)),
    };
}

std::string FluidSolver::BoxWithHoleScenario::GetName() {
    return "Box with hole";
}
