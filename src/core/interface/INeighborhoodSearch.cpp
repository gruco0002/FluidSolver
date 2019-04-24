//
// Created by corbi on 24.04.2019.
//

#include "INeighborhoodSearch.hpp"
#include <core/FluidSolverException.hpp>

void
FluidSolver::INeighborhoodSearch::FindNeighbors(FluidSolver::IParticleCollection *particleCollection, float radius) {
    throw FluidSolverException("Neighborhood Search: Unsupported operation!");
}

bool FluidSolver::INeighborhoodSearch::SupportsGlobalNeighborhoodFinding() {
    return false;
}
