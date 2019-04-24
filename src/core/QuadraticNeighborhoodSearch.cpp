//
// Created by corbi on 24.04.2019.
//

#include "QuadraticNeighborhoodSearch.hpp"

void FluidSolver::QuadraticNeighborhoodSearch::FindNeighbors(uint32_t particleIndex,
                                                             FluidSolver::IParticleCollection *particleCollection,
                                                             float radius) {

    // create bucket if not existing and / or clear it
    if (neighbors.find(particleIndex) == neighbors.end())
        neighbors[particleIndex] = std::vector<uint32_t>();
    neighbors[particleIndex].clear();

    // calculate neighbors
    glm::vec2 position = particleCollection->GetPosition(particleIndex);
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {


        glm::vec2 distVec = position - particleCollection->GetPosition(i);
        if (glm::length(distVec) <= radius) {
            // this is a neighbor, add it
            neighbors[particleIndex].push_back(i);
        }

    }

}

std::vector<uint32_t> &
FluidSolver::QuadraticNeighborhoodSearch::GetParticleNeighbors(uint32_t particleIndex) {
    return neighbors[particleIndex];
}
