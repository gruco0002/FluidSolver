#include "QuadraticNeighborhoodSearchDynamicAllocated.hpp"

void FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::FindNeighbors(uint32_t particleIndex,
                                                                             FluidSolver::IParticleCollection *particleCollection,
                                                                             float radius) {


    // delete old neighbors
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

FluidSolver::Neighbors
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::GetParticleNeighbors(uint32_t particleIndex) {
    return FluidSolver::Neighbors(&(neighbors[particleIndex][0]), neighbors[particleIndex].size());
}

void FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::SetParticleCount(uint32_t particleCount) {
    for (uint32_t i = 0; i < particleCount; i++) {
        // check if already allocated
        if (neighbors.find(i) == neighbors.end())
            neighbors[i] = std::vector<uint32_t>();
    }
}
