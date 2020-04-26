#include "INeighborhoodSearch.hpp"
#include <core/FluidSolverException.hpp>


FluidSolver::Neighbors::iterator FluidSolver::Neighbors::begin() {
    return FluidSolver::Neighbors::iterator(this, GetBegin());
}

FluidSolver::Neighbors::iterator FluidSolver::Neighbors::end() {
    return FluidSolver::Neighbors::iterator(this, GetEnd());
}

size_t FluidSolver::Neighbors::size() const {
    return GetEnd() - GetBegin();
}

size_t FluidSolver::Neighbors::GetEnd() {
    const FluidSolver::Neighbors *test = this;
    return test->GetEnd();
}

size_t FluidSolver::Neighbors::GetBegin() {
    const FluidSolver::Neighbors *test = this;
    return test->GetBegin();
}

FluidSolver::Neighbors::pointer
FluidSolver::Neighbors::GetPointer(FluidSolver::Neighbors::NeighborsIterator::iteratorPosition_t iteratorPosition) {
    const FluidSolver::Neighbors *test = this;
    return test->GetPointer(iteratorPosition);
}

FluidSolver::Neighbors::const_iterator FluidSolver::Neighbors::begin() const {
    return FluidSolver::Neighbors::const_iterator(this, GetBegin());
}

FluidSolver::Neighbors::const_iterator FluidSolver::Neighbors::end() const {
    return FluidSolver::Neighbors::const_iterator(this, GetEnd());
}

FluidSolver::INeighborhoodSearch::INeighborhoodSearch(FluidSolver::IParticleCollection *particleCollection,
                                                      float radius) {
    this->particleCollection = particleCollection;
    this->radius = radius;
}

FluidSolver::IParticleCollection *FluidSolver::INeighborhoodSearch::GetParticleCollection() {
    return particleCollection;
}

float FluidSolver::INeighborhoodSearch::GetRadius() {
    return radius;
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
        FluidSolver::Neighbors::NeighborsIterator::iteratorPosition_t iteratorPosition) const {
    return firstNeighbor + iteratorPosition;
}

size_t FluidSolver::NeighborsCompact::GetEnd() const {
    return neighborCount;
}

size_t FluidSolver::NeighborsCompact::GetBegin() const {
    return 0;
}

FluidSolver::NeighborsCompact::NeighborsCompact(FluidSolver::Neighbors::pointer firstNeighbor,
                                                particleAmount_t neighborCount) {
    this->firstNeighbor = firstNeighbor;
    this->neighborCount = neighborCount;
}

FluidSolver::NeighborsCompactData::NeighborsCompactData(std::vector<particleIndex_t> &neighbors) : NeighborsCompact(
        this->neighbors.data(), this->neighbors.size()) {
    this->neighbors = std::move(neighbors);
    this->firstNeighbor = this->neighbors.data();
}

FluidSolver::Neighbors::ConstNeighborsIterator::ConstNeighborsIterator(const FluidSolver::Neighbors *nb,
                                                                       FluidSolver::Neighbors::ConstNeighborsIterator::iteratorPosition_t iteratorPosition) {
    this->neighbors = nb;
    this->iteratorPosition = iteratorPosition;

// fetch the current pointer
    this->currentPointer = this->neighbors->GetPointer(this->iteratorPosition);
}

bool FluidSolver::Neighbors::ConstNeighborsIterator::operator==(
        const FluidSolver::Neighbors::ConstNeighborsIterator &other) const {
    return iteratorPosition == other.iteratorPosition && neighbors == other.neighbors;
}

bool FluidSolver::Neighbors::ConstNeighborsIterator::operator!=(
        const FluidSolver::Neighbors::ConstNeighborsIterator &other) const {
    return !(*this == other);
}

const FluidSolver::Neighbors::T &FluidSolver::Neighbors::ConstNeighborsIterator::operator*() {
    return *currentPointer;
}

FluidSolver::Neighbors::ConstNeighborsIterator &FluidSolver::Neighbors::ConstNeighborsIterator::operator++() {
    ++iteratorPosition;
    this->currentPointer = neighbors->GetPointer(iteratorPosition);
    return *this;
}

const FluidSolver::Neighbors::ConstNeighborsIterator FluidSolver::Neighbors::ConstNeighborsIterator::operator++(int) {
    // create a copy of the current iterator for return purposes
    FluidSolver::Neighbors::ConstNeighborsIterator clone(*this);

    // increment this instance of the iterator
    ++iteratorPosition;
    this->currentPointer = neighbors->GetPointer(iteratorPosition);

    // return the clone representing the old state
    return clone;
}
