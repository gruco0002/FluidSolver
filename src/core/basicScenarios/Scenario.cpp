//
// Created by corbi on 28.05.2019.
//
#include <core/fluidSolver/particleCollection/ParticleCollection.hpp>
#include "core/basicScenarios/Scenario.hpp"


FluidSolver::IParticleCollection *FluidSolver::Scenario::GenerateScenario(float restDensity) {
    auto tmp = std::vector<FluidSolver::FluidParticle>();
    auto particleCollection = new FluidSolver::ParticleCollection();
    particleCollection->AddParticles(tmp);

    this->ResetData(particleCollection, restDensity);

    return particleCollection;
}

float FluidSolver::Scenario::GetParticleSize() {
    return 1.0f;
}

bool FluidSolver::Scenario::ParticleCollectionTypeSupported(IParticleCollection *particleCollection) {
    return dynamic_cast<FluidSolver::ParticleCollection *>(particleCollection) != nullptr;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::Scenario::GetSimulationModifiers() {
    return std::vector<FluidSolver::ISimulationModifier *>();
}
