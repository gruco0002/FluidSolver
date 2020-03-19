
#include "IParticleCollection.hpp"

namespace FluidSolver {
    bool FluidParticle::operator==(const FluidParticle &rhs) const {
        return Position == rhs.Position &&
               Velocity == rhs.Velocity &&
               Acceleration == rhs.Acceleration &&
               Mass == rhs.Mass &&
               Pressure == rhs.Pressure &&
               Density == rhs.Density &&
               Type == rhs.Type &&
               PredictedVelocity == rhs.PredictedVelocity &&
               NonPressureAcceleration == rhs.NonPressureAcceleration &&
               SourceTerm == rhs.SourceTerm &&
               DiagonalElement == rhs.DiagonalElement;
    }

    bool FluidParticle::operator!=(const FluidParticle &rhs) const {
        return !(rhs == *this);
    }
}

void FluidSolver::IParticleCollection::InsertionSort(
        const FluidSolver::IParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // pre calculate the keys:
    PrecalculateSortKeys(sortKeyFunction);

    // start insertion sort
    for (size_t i = 1; i < GetSize(); i++) {
        auto key = GetSortKey(i);
        size_t j = i - 1;
        while (j >= 0 && GetSortKey(j) > key) {
            SwapElements(j, j + 1);
            j--;
        }
    }

}

void
FluidSolver::IParticleCollection::MergeSort(
        const FluidSolver::IParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // pre calculate the keys:
    PrecalculateSortKeys(sortKeyFunction);

    // define merge, left index is inclusive, right index is exclusive
    auto merge = [=](size_t left, size_t half, size_t right) {
        size_t leftIndex = left;
        size_t rightIndex = half;

        if (GetSortKey(rightIndex - 1) <= GetSortKey(rightIndex))
            return; // everything was already sorted

        while (leftIndex < half && rightIndex < right) {
            if (GetSortKey(leftIndex) <= GetSortKey(rightIndex)) {
                leftIndex++; // the element is already in the right place
            } else {
                // move the item by continous swapping to the target, then adapt half and other indices accordingly
                size_t movingIndex = rightIndex;
                while (movingIndex > leftIndex) {
                    SwapElements(movingIndex, movingIndex - 1);
                    movingIndex--;
                }

                rightIndex++;
                half++;
                leftIndex++;
            }
        }

    };


    // define merge sort, left index is inclusive, right index is exclusive
    std::function<void(size_t, size_t)> mergesort = [=](size_t left, size_t right) {
        if (right - left <= 1)
            return;

        size_t half = left + (right - left) / 2;
        mergesort(left, half);
        mergesort(half, right);

        merge(left, half, right);
    };

    // start mergesort
    mergesort(0, GetSize());
}

void FluidSolver::IParticleCollection::AddParticles(const std::vector<FluidParticle> &particles) {
    for (auto particle: particles)
        AddParticle(particle);
}
