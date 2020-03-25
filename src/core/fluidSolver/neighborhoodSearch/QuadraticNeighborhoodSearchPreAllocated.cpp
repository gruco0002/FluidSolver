#include "QuadraticNeighborhoodSearchPreAllocated.hpp"
#include "core/FluidSolverException.hpp"

void FluidSolver::QuadraticNeighborhoodSearchPreAllocated::FindNeighbors() {

    // start by checking sizes
    auto particleCount = particleCollection->GetSize();
    if (neighborsCount.size() < particleCount) neighborsCount.resize(particleCount);
    if (neighborsStart.size() < particleCount) neighborsStart.resize(particleCount);

    // calculate neighbor count
#pragma omp parallel for
    for (particleIndex_t i = 0; i < particleCount; i++) {

        if (particleCollection->GetParticleType(i) == ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }

        particleAmount_t count = 0;
        glm::vec2 position = particleCollection->GetPosition(i);

        // for the particle i, find all neighbors j
#pragma omp parallel for
        for (particleIndex_t j = 0; j < particleCount; j++) {
            glm::vec2 distVec = position - particleCollection->GetPosition(j);
            if (glm::length(distVec) <= radius) {
                // this is a neighbor, increase count
                count++;
            }
        }

        // save the count value for particle i
        neighborsCount[i] = count;
    }

    // calculate neighbor start
#pragma omp parallel for
    for (particleIndex_t i = 0; i < particleCount; i++) {
        particleAmount_t sum = 0;
        for (particleAmount_t j = 0; j < i; j++) {
            sum = sum + neighborsCount[j];
        }
        neighborsStart[i] = sum;
    }

    // calculate total neighbor count
    particleAmount_t totalNeighborCount = neighborsStart[particleCount - 1] + neighborsCount[particleCount - 1];

    // check if storage is enough and resize it if necessary
    if (neighbors.size() < totalNeighborCount) neighbors.resize(totalNeighborCount);

    // find neighbors and insert them into the neighbor array
#pragma omp parallel for
    for (particleIndex_t i = 0; i < particleCount; i++) {

        if (particleCollection->GetParticleType(i) == ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }

        glm::vec2 position = particleCollection->GetPosition(i);
        size_t currentArrayPosition = neighborsStart[i];

        // for the particle i, find all neighbors j
        for (particleIndex_t j = 0; j < particleCount; j++) {
            glm::vec2 distVec = position - particleCollection->GetPosition(j);
            if (glm::length(distVec) <= radius) {
                // this is a neighbor, add it to the list
                neighbors[currentArrayPosition] = j;
                currentArrayPosition++;
            }
        }

    }
}

FluidSolver::NeighborsCompact
FluidSolver::QuadraticNeighborhoodSearchPreAllocated::GetNeighbors(FluidSolver::particleIndex_t particleIndex) {
    return NeighborsCompact(&neighbors[neighborsStart[particleIndex]], neighborsCount[particleIndex]);
}

FluidSolver::NeighborsCompactData
FluidSolver::QuadraticNeighborhoodSearchPreAllocated::GetNeighbors(glm::vec2 position) {
    // TODO: implement
    throw std::logic_error("Not implemented");
}

FluidSolver::QuadraticNeighborhoodSearchPreAllocated::QuadraticNeighborhoodSearchPreAllocated(
        FluidSolver::IParticleCollection *particleCollection, float radius) : INeighborhoodSearch(particleCollection,
                                                                                                  radius) {}
