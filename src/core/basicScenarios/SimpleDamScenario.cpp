//
// Created by corbi on 28.05.2019.
//

#include <core/fluidSolver/particleCollection/SimpleParticleCollection.hpp>
#include "SimpleDamScenario.hpp"

void
FluidSolver::SimpleDamScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {
    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    auto coll = dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection);

    float mass = restDensity * GetParticleSize() * GetParticleSize();


    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    for (int x = -12; x <= 12; x++) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = glm::vec2((float) x, (float) -5);
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;
        particles.push_back(p);

        p.Position = glm::vec2((float) x, -6.0f);
        particles.push_back(p);

        p.Position = glm::vec2((float) x, -7.0f);
        particles.push_back(p);
    }

    // left + right boundary
    for (int y = 5; y > -5; y--) {

        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = restDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -10, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -11, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -12, (float) y);
        particles.push_back(p);

        p.Position = glm::vec2((float) 10, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 11, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 12, (float) y);
        particles.push_back(p);

    }


    for (int x = -3; x <= 3; x++) {
        for (int y = 3; y >= 0; y--) {
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

int FluidSolver::SimpleDamScenario::GetParticleCountX() {
    return 26;
}

int FluidSolver::SimpleDamScenario::GetParticleCountY() {
    return 20;
}
