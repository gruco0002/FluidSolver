#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include <core/fluidSolver/particleCollection/CompactParticleCollection.hpp>
#include "core/basicScenarios/Scenario.hpp"


FluidSolver::IParticleCollection *FluidSolver::Scenario::GenerateScenario(float restDensity) {
    auto tmp = std::vector<FluidSolver::FluidParticle>();
    auto particleCollection = CreateEmptyParticleCollection();
    particleCollection->AddParticles(tmp);

    this->ResetData(particleCollection, restDensity);

    return particleCollection;
}

float FluidSolver::Scenario::GetParticleSize() {
    return 1.0f;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::Scenario::GetSimulationModifiers() {
    return std::vector<FluidSolver::ISimulationModifier *>();
}

FluidSolver::IParticleCollection *FluidSolver::Scenario::CreateEmptyParticleCollection() {
    return new FluidSolver::CompactParticleCollection();
}
