//
// Created by corbi on 28.05.2019.
//

#include <core/fluidSolver/particleCollection/SimpleParticleCollection.hpp>
#include <core/simulationModifiers/DeathBox.hpp>
#include "SimpleBoxScenario.hpp"

void
FluidSolver::SimpleBoxScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    auto coll = dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection);

    float mass = restDensity * GetParticleSize() * GetParticleSize();


    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    for (int x = -23; x <= 12; x++) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) x, (float) -14);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -15);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -16);
        particles.push_back(p);
    }

    // left boundary
    for (int y = 20; y >= -13; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -21, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -22, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -23, (float) y);
        particles.push_back(p);
    }

    // right boundary
    for (int y = 20; y >= -13; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

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
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
            particles.push_back(p);
        }
    }

    coll->SetParticles(particles);

}

int FluidSolver::SimpleBoxScenario::GetParticleCountX() {
    return 45;
}

int FluidSolver::SimpleBoxScenario::GetParticleCountY() {
    return 60;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::SimpleBoxScenario::GetSimulationModifiers() {
    return {new DeathBox(25, -25, -20, 15)};
}
