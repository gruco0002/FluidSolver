#include "ZIndexGridSorter.hpp"
#include "libmorton/libmorton/include/morton.h"

FluidSolver::ZIndexGridSorter::ZIndexGridSorter(float gridCellSize,
                                                FluidSolver::IParticleCollection *particleCollection) : GridCellSize(
        gridCellSize), particleCollection(particleCollection) {}

void FluidSolver::ZIndexGridSorter::CalculateGridCoordinates(FluidSolver::particleIndex_t particleIndex,
                                                             int32_t &gridX, int32_t &gridY) {

    auto pos = particleCollection->GetPosition(particleIndex);
    gridX = (int32_t) floor(pos.x / GridCellSize); // a simple cast to int would round towards zero, hence cause errors
    gridY = (int32_t) floor(pos.y / GridCellSize);

}

uint64_t FluidSolver::ZIndexGridSorter::CalculateSortKey(FluidSolver::particleIndex_t particleIndex) {

    if (particleCollection->GetParticleType(particleIndex) == ParticleType::ParticleTypeDead) {
        return std::numeric_limits<uint64_t>::max();
    }

    uint32_t x;
    uint32_t y;
    CalculateGridCoordinatesUnsigned(particleIndex, x, y);
    return libmorton::morton2D_64_encode(x, y);
}

void FluidSolver::ZIndexGridSorter::CalculateGridCoordinatesUnsigned(FluidSolver::particleIndex_t particleIndex,
                                                                     uint32_t &gridX, uint32_t &gridY) {

    auto pos = particleCollection->GetPosition(particleIndex);
    gridX = (uint32_t) (((int64_t) floor(pos.x / GridCellSize)) +
                        ((int64_t) std::numeric_limits<int32_t>::max())); // a simple cast to int would round towards zero, hence cause errors
    gridY = (uint32_t) (((int64_t) floor(pos.y / GridCellSize)) + ((int64_t) std::numeric_limits<int32_t>::max()));

}

void FluidSolver::ZIndexGridSorter::SortInitially() {
    particleCollection->MergeSort([this](particleIndex_t index) { return this->CalculateSortKey(index); });
}

void FluidSolver::ZIndexGridSorter::Sort() {
    particleCollection->InsertionSort([this](particleIndex_t index) { return this->CalculateSortKey(index); });
}
