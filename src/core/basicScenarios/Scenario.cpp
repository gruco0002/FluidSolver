//
// Created by corbi on 28.05.2019.
//
#include <core/SimpleParticleCollection.hpp>
#include "core/basicScenarios/Scenario.hpp"


FluidSolver::IParticleCollection *FluidSolver::Scenario::GenerateScenario(float restDensity) {
    auto tmp = std::vector<FluidSolver::SimpleParticleCollection::FluidParticle>();
    auto particleCollection = new FluidSolver::SimpleParticleCollection(tmp);

    this->ResetData(particleCollection, restDensity);

    return particleCollection;
}

float FluidSolver::Scenario::GetParticleSize() {
    return 1.0f;
}

bool FluidSolver::Scenario::ParticleCollectionTypeSupported(IParticleCollection *particleCollection) {
    return dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection) != nullptr;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::Scenario::GetSimulationModifiers() {
    return std::vector<FluidSolver::ISimulationModifier *>();
}
