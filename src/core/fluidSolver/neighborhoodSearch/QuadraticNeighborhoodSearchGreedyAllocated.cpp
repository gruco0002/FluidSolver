#include "QuadraticNeighborhoodSearchGreedyAllocated.hpp"

void FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::FindNeighbors() {

    // for each particle: create a bucket if not existing
    while (bucketsCreatedUntilIndex < particleCollection->GetSize()) {
        neighbors[bucketsCreatedUntilIndex] = std::pair<particleAmount_t, std::vector<particleIndex_t >>(0,
                                                                                                         std::vector<particleIndex_t>());
        bucketsCreatedUntilIndex++;
    }

    // execute the actual neighborhood search
#pragma omp parallel for
    for (particleIndex_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {
        particleAmount_t neighborCount = 0;

        // calculate neighbors
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        for (particleIndex_t i = 0; i < particleCollection->GetSize(); i++) {
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

}

std::shared_ptr<FluidSolver::Neighbors>
FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::GetNeighbors(FluidSolver::particleIndex_t particleIndex) {
    return std::shared_ptr<Neighbors>(
            new NeighborsCompact(neighbors[particleIndex].second.data(), neighbors[particleIndex].first));
}

std::shared_ptr<FluidSolver::Neighbors>
FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated::GetNeighbors(glm::vec2 position) {
    // TODO: implement
    throw std::logic_error("Not implemented");
    return std::shared_ptr<Neighbors>();
}
