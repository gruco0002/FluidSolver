//
// Created by corbi on 08.01.2020.
//

#include "NormalParticleSelection.hpp"

bool FluidSolver::NormalParticleSelection::IsParticleSelected(uint32_t particleIndex,
                                                              FluidSolver::IParticleCollection *particleCollection) {
    auto type = particleCollection->GetParticleType(particleIndex);
    return type == IParticleCollection::ParticleTypeNormal;
}
