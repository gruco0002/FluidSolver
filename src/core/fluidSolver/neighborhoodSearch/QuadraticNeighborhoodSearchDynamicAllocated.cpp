#include "QuadraticNeighborhoodSearchDynamicAllocated.hpp"


void FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::FindNeighbors() {

    // create map entry for particles if not already existing
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        // check if already allocated
        if (neighbors.find(i) == neighbors.end())
            neighbors[i] = std::vector<uint32_t>();
    }

    // find the neighbors by quadratic search
#pragma omp parallel for
    for (uint32_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {

        // delete old neighbors of particles in the map
        neighbors[particleIndex].clear();

        // calculate neighbors for this particle
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            glm::vec2 distVec = position - particleCollection->GetPosition(i);
            if (glm::length(distVec) <= radius) {
                // this is a neighbor: add it to the list inside the map
                neighbors[particleIndex].push_back(i);

            }

        }
    }
}

std::shared_ptr<FluidSolver::Neighbors>
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::GetNeighbors(uint32_t particleIndex) {
    return std::shared_ptr<Neighbors>(new NeighborsCompact(neighbors[particleIndex].data(), neighbors[particleIndex].size()));
}

std::shared_ptr<FluidSolver::Neighbors>
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::GetNeighbors(glm::vec2 position) {
    return std::shared_ptr<Neighbors>();
}

