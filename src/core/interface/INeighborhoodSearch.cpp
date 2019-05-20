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

FluidSolver::Neighbors::iterator FluidSolver::Neighbors::begin() {
    return FluidSolver::Neighbors::iterator(*this, 0);
}

FluidSolver::Neighbors::iterator FluidSolver::Neighbors::end() {
    return FluidSolver::Neighbors::iterator(*this, _size);
}

FluidSolver::NeighborsIterator::NeighborsIterator(FluidSolver::Neighbors &nb, size_t position) : neighbors(nb),
                                                                                                 position(position) {

}

bool FluidSolver::NeighborsIterator::operator==(const FluidSolver::NeighborsIterator &other) const {
    return position == other.position && &neighbors == &other.neighbors;
}

bool FluidSolver::NeighborsIterator::operator!=(const FluidSolver::NeighborsIterator &other) const {
    return !(*this == other);
}

FluidSolver::Neighbors::T &FluidSolver::NeighborsIterator::operator*() {
    return neighbors.start[position];
}

FluidSolver::NeighborsIterator &FluidSolver::NeighborsIterator::operator++() {
    ++position;
    return *this;
}

const FluidSolver::NeighborsIterator FluidSolver::NeighborsIterator::operator++(int) {
    FluidSolver::NeighborsIterator clone(*this);
    ++position;
    return clone;
}

FluidSolver::Neighbors::Neighbors(FluidSolver::Neighbors::T *start, size_t size) : start(start), _size(size) {

}

size_t FluidSolver::Neighbors::size() {
    return _size;
}
