//
// Created by corbi on 08.01.2020.
//

#include "ParticleSelection.hpp"


void FluidSolver::ParticleSelection::AddParticleToSelection(uint32_t particleIndex) {
    selectedParticles.insert(particleIndex);
}

void FluidSolver::ParticleSelection::ClearSelection() {
    selectedParticles.clear();
}

bool FluidSolver::ParticleSelection::IsParticleSelected(uint32_t particleIndex,
                                                        FluidSolver::IParticleCollection *particleCollection) {
    if (selectedParticles.empty()) return true;
    return selectedParticles.find(particleIndex) != selectedParticles.end();
}