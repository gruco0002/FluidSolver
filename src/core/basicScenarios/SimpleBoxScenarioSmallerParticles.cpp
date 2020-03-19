//
// Created by corbi on 25.11.2019.
//

#include <core/fluidSolver/particleCollection/ParticleCollection.hpp>
#include <core/simulationModifiers/DeathBox.hpp>
#include "SimpleBoxScenarioSmallerParticles.hpp"

void FluidSolver::SimpleBoxScenarioSmallerParticles::ResetData(FluidSolver::IParticleCollection *particleCollection,
                                                               float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))
        return;

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

        for (float offsetX = 0.0f; offsetX < 1.0f; offsetX += Size) {
            for (float offsetY = 0.0f; offsetY < 1.0f; offsetY += Size) {
                p.Position = glm::vec2((float) x + offsetX, (float) -14 + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) x + offsetX, (float) -15 + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) x + offsetX, (float) -16 + offsetY);
                particles.push_back(p);
            }
        }
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
        continue;
        for (float offsetX = 0.0f; offsetX < 1.0f; offsetX += Size) {
            for (float offsetY = 0.0f; offsetY < 1.0f; offsetY += Size) {
                p.Position = glm::vec2((float) -21 + offsetX, (float) y + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) -22 + offsetX, (float) y + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) -23 + offsetX, (float) y + offsetY);
                particles.push_back(p);
            }
        }
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

        continue;
        for (float offsetX = 0.0f; offsetX < 1.0f; offsetX += Size) {
            for (float offsetY = 0.0f; offsetY < 1.0f; offsetY += Size) {
                p.Position = glm::vec2((float) 10 + offsetX, (float) y + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) 11 + offsetX, (float) y + offsetY);
                particles.push_back(p);
                p.Position = glm::vec2((float) 12 + offsetX, (float) y + offsetY);
                particles.push_back(p);
            }
        }

    }

    // particles
    for (int x = 0; x <= 4; x++) {
        for (int y = 0; y >= -x; y--) {
            // normal particle
            FluidSolver::FluidParticle p;

            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;

            // for (float offsetX = 0.0f; offsetX < 1.0f; offsetX += Size) {
            //   for (float offsetY = 0.0f; offsetY < 1.0f; offsetY += Size) {
            p.Position = glm::vec2((float) x, (float) y);
            particles.push_back(p);
            //    }
            //  }
        }
    }

    coll->AddParticles(particles);
}

int FluidSolver::SimpleBoxScenarioSmallerParticles::GetParticleCountX() {
    return 45;
}

int FluidSolver::SimpleBoxScenarioSmallerParticles::GetParticleCountY() {
    return 60;
}

std::vector<FluidSolver::ISimulationModifier *>
FluidSolver::SimpleBoxScenarioSmallerParticles::GetSimulationModifiers() {
    return {new DeathBox(25, -25, -20, 15)};
}

float FluidSolver::SimpleBoxScenarioSmallerParticles::GetParticleSize() {
    return Size;
}

std::string FluidSolver::SimpleBoxScenarioSmallerParticles::GetName() {
    return "Simple box (small particles)";
}
