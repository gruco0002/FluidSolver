#include "QuadraticNeighborhoodSearchGreedyAllocated.hpp"

void FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::FindNeighbors(uint32_t particleIndex,
                                                             FluidSolver::IParticleCollection *particleCollection,
                                                             float radius) {

    uint32_t neighborCount = 0;
    // calculate neighbors
    glm::vec2 position = particleCollection->GetPosition(particleIndex);
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {


        glm::vec2 distVec = position - particleCollection->GetPosition(i);
        if (glm::length(distVec) <= radius) {
            // this is a neighbor, add it

            if (neighbors[particleIndex].second.size() <= neighborCount) {
                neighbors[particleIndex].second.push_back(i);
            } else {
                neighbors[particleIndex].second[neighborCount] = i;
            }
            neighborCount++;
        }

    }
    neighbors[particleIndex].first = neighborCount;

}

FluidSolver::Neighbors
FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::GetParticleNeighbors(uint32_t particleIndex) {
    return FluidSolver::Neighbors(&(neighbors[particleIndex].second[0]), neighbors[particleIndex].first);
}

void FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::SetParticleCount(uint32_t particleCount) {
    // create buckets if not existing and / or clear it
    while (bucketsCreatedUntilIndex < particleCount) {
        neighbors[bucketsCreatedUntilIndex] = std::pair<uint32_t, std::vector<uint32_t >>(0, std::vector<uint32_t>());
        bucketsCreatedUntilIndex++;
    }

}
