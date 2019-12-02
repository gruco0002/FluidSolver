//
// Created by corbi on 28.05.2019.
//

#include <core/fluidSolver/particleCollection/SimpleParticleCollection.hpp>
#include "HugeDamScenario.hpp"

void FluidSolver::HugeDamScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    auto coll = dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection);

    float mass = restDensity * GetParticleSize() * GetParticleSize();

    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    for (int x = -53; x <= 53; x++) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) x, (float) -50);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -51);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -52);
        particles.push_back(p);
    }

    // left boundary
    for (int y = 20; y > -50; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -50, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -51, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -52, (float) y);
        particles.push_back(p);
    }

    // right boundary
    for (int y = 20; y > -50; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) 50, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 51, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 52, (float) y);
        particles.push_back(p);

    }


    for (int x = -20; x <= 0; x++) {
        for (int y = 20; y >= 0; y--) {
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

int FluidSolver::HugeDamScenario::GetParticleCountX() {
    return 106;
}

int FluidSolver::HugeDamScenario::GetParticleCountY() {
    return 106;
}

std::string FluidSolver::HugeDamScenario::GetName() {
    return "Huge dam";
}
