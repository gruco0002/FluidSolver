//
// Created by corbi on 11.12.2019.
//

#include "SimpleBoxScenarioLargerParticles.hpp"

#include <core/fluidSolver/particleCollection/ParticleCollection.hpp>
#include <core/simulationModifiers/DeathBox.hpp>

void
FluidSolver::SimpleBoxScenarioLargerParticles::ResetData(FluidSolver::IParticleCollection *particleCollection,
                                                         float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    auto coll = dynamic_cast<FluidSolver::ParticleCollection *>(particleCollection);

    float mass = restDensity * GetParticleSize() * GetParticleSize();


    // generate a simple boundary
    std::vector<FluidSolver::FluidParticle> particles;

    for (int x = -23; x <= 12; x++) {
        FluidSolver::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) x * Factor, (float) -14 * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) x * Factor, (float) -15 * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) x * Factor, (float) -16 * Factor);
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
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -21 * Factor, (float) y * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) -22 * Factor, (float) y * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) -23 * Factor, (float) y * Factor);
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
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) 10 * Factor, (float) y * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) 11 * Factor, (float) y * Factor);
        particles.push_back(p);
        p.Position = glm::vec2((float) 12 * Factor, (float) y * Factor);
        particles.push_back(p);

    }

    // particles
    for (int x = -20; x <= 0; x++) {
        for (int y = 7; y >= -13; y--) {
            // normal particle
            FluidSolver::FluidParticle p;
            p.Position = glm::vec2((float) x * Factor, (float) y * Factor);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
            particles.push_back(p);
        }
    }

    coll->AddParticles(particles);

}

int FluidSolver::SimpleBoxScenarioLargerParticles::GetParticleCountX() {
    return 45 * Factor;
}

int FluidSolver::SimpleBoxScenarioLargerParticles::GetParticleCountY() {
    return 60 * Factor;
}

std::vector<FluidSolver::ISimulationModifier *>
FluidSolver::SimpleBoxScenarioLargerParticles::GetSimulationModifiers() {
    return {new DeathBox(25 * Factor, -25 * Factor, -20 * Factor, 15 * Factor)};
}

std::string FluidSolver::SimpleBoxScenarioLargerParticles::GetName() {
    return "Simple box (larger particles)";
}

float FluidSolver::SimpleBoxScenarioLargerParticles::GetParticleSize() {
    return Factor;
}
