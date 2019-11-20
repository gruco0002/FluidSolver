//
// Created by gruberc on 17.05.19.
//

#include "QuadraticNeighborhoodSearchPreAllocated.hpp"
#include "core/FluidSolverException.hpp"



void FluidSolver::QuadraticNeighborhoodSearchPreAllocated::FindNeighbors(
        FluidSolver::IParticleCollection *particleCollection, float radius) {


    // start by checking sizes
    auto particleCount = particleCollection->GetSize();
    if (neighborsCount.size() < particleCount) neighborsCount.resize(particleCount);
    if (neighborsStart.size() < particleCount) neighborsStart.resize(particleCount);

    // calculate neighbor count
    #pragma omp parallel for
    for (int64_t  i = 0; i < particleCount; i++) {

        if (particleCollection->GetParticleType(i) == IParticleCollection::ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }

        uint32_t count = 0;
        glm::vec2 position = particleCollection->GetPosition(i);

        // for the particle i, find all neighbors j
        #pragma omp parallel for
        for (int64_t  j = 0; j < particleCount; j++) {
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
    for (int64_t  i = 0; i < particleCount; i++) {
        uint32_t sum = 0;
        for (uint32_t j = 0; j < i; j++) {
            sum = sum + neighborsCount[j];
        }
        neighborsStart[i] = sum;
    }

    // calculate total neighbor count
    uint32_t totalNeighborCount = neighborsStart[particleCount - 1] + neighborsCount[particleCount - 1];

    // check if storage is enough and resize it if necessary
    if (neighbors.size() < totalNeighborCount) neighbors.resize(totalNeighborCount);

    // find neighbors and insert them into the neighbor array
    #pragma omp parallel for
    for (int64_t  i = 0; i < particleCount; i++) {

        if (particleCollection->GetParticleType(i) == IParticleCollection::ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }

        glm::vec2 position = particleCollection->GetPosition(i);
        uint32_t currentArrayPosition = neighborsStart[i];

        // for the particle i, find all neighbors j
        for (uint32_t j = 0; j < particleCount; j++) {
            glm::vec2 distVec = position - particleCollection->GetPosition(j);
            if (glm::length(distVec) <= radius) {
                // this is a neighbor, add it to the list
                neighbors[currentArrayPosition] = j;
                currentArrayPosition++;
            }
        }

    }


}

void FluidSolver::QuadraticNeighborhoodSearchPreAllocated::FindNeighbors(uint32_t particleIndex,
                                                                         FluidSolver::IParticleCollection *particleCollection,
                                                                         float radius) {
    throw FluidSolver::FluidSolverException("Not for usage");
}

FluidSolver::Neighbors
FluidSolver::QuadraticNeighborhoodSearchPreAllocated::GetParticleNeighbors(uint32_t particleIndex) {
    return Neighbors(&neighbors[neighborsStart[particleIndex]], neighborsCount[particleIndex]);
}
