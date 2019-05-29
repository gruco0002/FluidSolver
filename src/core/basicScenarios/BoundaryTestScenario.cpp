//
// Created by corbi on 28.05.2019.
//

#include <core/SimpleParticleCollection.hpp>
#include "BoundaryTestScenario.hpp"

void
FluidSolver::BoundaryTestScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {

    if (!this->ParticleCollectionTypeSupported(particleCollection))return;

    auto coll = dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection);

    float mass = restDensity * GetParticleSize() * GetParticleSize();

    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (int y = -5; y >= -7; y--) {
        for (int x = -10; x <= 10; x++) {
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;
            particles.push_back(p);
        }
    }

    // normal particle
    FluidSolver::SimpleParticleCollection::FluidParticle p;
    p.Position = glm::vec2(0.0f);
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = restDensity;
    p.Mass = mass;
    p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
    particles.push_back(p);

    coll->SetParticles(particles);

}

int FluidSolver::BoundaryTestScenario::GetParticleCountX() {
    return 20;
}

int FluidSolver::BoundaryTestScenario::GetParticleCountY() {
    return 20;
}