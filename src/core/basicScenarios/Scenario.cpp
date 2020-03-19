#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include "core/basicScenarios/Scenario.hpp"


FluidSolver::IParticleCollection *FluidSolver::Scenario::GenerateScenario(float restDensity) {
    auto tmp = std::vector<FluidSolver::FluidParticle>();
    auto particleCollection = new FluidSolver::StripedParticleCollection();
    particleCollection->AddParticles(tmp);

    this->ResetData(particleCollection, restDensity);

    return particleCollection;
}

float FluidSolver::Scenario::GetParticleSize() {
    return 1.0f;
}

bool FluidSolver::Scenario::ParticleCollectionTypeSupported(IParticleCollection *particleCollection) {
    return dynamic_cast<FluidSolver::StripedParticleCollection *>(particleCollection) != nullptr;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::Scenario::GetSimulationModifiers() {
    return std::vector<FluidSolver::ISimulationModifier *>();
}
