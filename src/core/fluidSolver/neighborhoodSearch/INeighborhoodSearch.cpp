#include "INeighborhoodSearch.hpp"
#include <core/FluidSolverException.hpp>


FluidSolver::Neighbors::iterator FluidSolver::Neighbors::begin() {
    return FluidSolver::Neighbors::iterator(this, GetBegin());
}

FluidSolver::Neighbors::iterator FluidSolver::Neighbors::end() {
    return FluidSolver::Neighbors::iterator(this, GetEnd());
}

size_t FluidSolver::Neighbors::size() {
    return GetEnd() - GetBegin();
}

FluidSolver::INeighborhoodSearch::INeighborhoodSearch(FluidSolver::IParticleCollection *particleCollection,
                                                      float radius) {
    this->particleCollection = particleCollection;
    this->radius = radius;
}

FluidSolver::Neighbors::NeighborsIterator::NeighborsIterator(FluidSolver::Neighbors *nb,
                                                             NeighborsIterator::iteratorPosition_t iteratorPosition) {
    this->iteratorPosition = iteratorPosition;
    this->neighbors = nb;

    // fetch the current pointer
    this->currentPointer = this->neighbors->GetPointer(this->iteratorPosition);
}

bool
FluidSolver::Neighbors::NeighborsIterator::operator==(const FluidSolver::Neighbors::NeighborsIterator &other) const {
    return iteratorPosition == other.iteratorPosition && neighbors == other.neighbors;
}

bool
FluidSolver::Neighbors::NeighborsIterator::operator!=(const FluidSolver::Neighbors::NeighborsIterator &other) const {
    return !(*this == other);
}

FluidSolver::Neighbors::T &FluidSolver::Neighbors::NeighborsIterator::operator*() {
    return *currentPointer;
}

FluidSolver::Neighbors::NeighborsIterator &FluidSolver::Neighbors::NeighborsIterator::operator++() {
    ++iteratorPosition;
    this->currentPointer = neighbors->GetPointer(iteratorPosition);
    return *this;
}

const FluidSolver::Neighbors::NeighborsIterator FluidSolver::Neighbors::NeighborsIterator::operator++(int) {
    // create a copy of the current iterator for return purposes
    FluidSolver::Neighbors::NeighborsIterator clone(*this);

    // increment this instance of the iterator
    ++iteratorPosition;
    this->currentPointer = neighbors->GetPointer(iteratorPosition);

    // return the clone representing the old state
    return clone;
}

FluidSolver::Neighbors::pointer FluidSolver::NeighborsCompact::GetPointer(
        FluidSolver::Neighbors::NeighborsIterator::iteratorPosition_t iteratorPosition) {
    return firstNeighbor + iteratorPosition;
}

size_t FluidSolver::NeighborsCompact::GetEnd() {
    return neighborCount;
}

size_t FluidSolver::NeighborsCompact::GetBegin() {
    return 0;
}

FluidSolver::NeighborsCompact::NeighborsCompact(FluidSolver::Neighbors::pointer firstNeighbor, size_t neighborCount) {
    this->firstNeighbor = firstNeighbor;
    this->neighborCount = neighborCount;
}
