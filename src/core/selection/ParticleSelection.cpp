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
    if (selectedParticles.empty()) return false;
    return selectedParticles.find(particleIndex) != selectedParticles.end();
}

size_t FluidSolver::ParticleSelection::GetSize() {
    return selectedParticles.size();
}

void FluidSolver::ParticleSelection::RemoveParticleFromSelection(uint32_t particleIndex) {
    selectedParticles.erase(particleIndex);
}
