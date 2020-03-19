#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include "BoundaryTestScenario.hpp"

void
FluidSolver::BoundaryTestScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {
    particleCollection->Clear();

    float mass = restDensity * GetParticleSize() * GetParticleSize();

    // generate a simple boundary
    std::vector<FluidSolver::FluidParticle> particles;
    for (int y = -5; y >= -7; y--) {
        for (int x = -10; x <= 10; x++) {
            FluidSolver::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = restDensity;
            p.Mass = mass;
            p.Type = FluidSolver::ParticleTypeBoundary;
            particles.push_back(p);
        }
    }

    // normal particle
    FluidSolver::FluidParticle p;
    p.Position = glm::vec2(0.0f);
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = restDensity;
    p.Mass = mass;
    p.Type = FluidSolver::ParticleTypeNormal;
    particles.push_back(p);

    particleCollection->AddParticles(particles);

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
