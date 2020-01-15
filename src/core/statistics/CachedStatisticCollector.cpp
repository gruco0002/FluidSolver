//
// Created by corbi on 05.01.2020.
//

#include "CachedStatisticCollector.hpp"
#include "CachedStatValue.hpp"

void FluidSolver::CachedStatisticCollector::CalculateData() {
    StatisticCollector::CalculateData();
    // after calculating the data, we save it to our cache
    CreateNewCacheEntry();
}

void FluidSolver::CachedStatisticCollector::ResetCache() {
    CurrentCacheDataSize = 0;
    CurrentCacheDataPointer = 0;
}

FluidSolver::CachedStatisticCollector::CachedStatisticCollector() : FluidSolver::StatisticCollector() {
    RemapStatValues();
    RefreshFieldVector();
    ResizeVectors();
}

void FluidSolver::CachedStatisticCollector::ResizeVectors() {
    for (auto tmp : Stats) {
        auto casted = dynamic_cast<CachedStatValue *>(tmp);
        if (casted != nullptr) {
            casted->Resize(CacheSize);
        }
    }
    Timesteps->Resize(CacheSize);
}

void FluidSolver::CachedStatisticCollector::CreateNewCacheEntry() {

    // timestep is added up if possible
    if (CurrentCacheDataSize == 0) {
        Timesteps->Set(getTimestep());
        Timesteps->SetValue(CurrentCacheDataPointer);
    } else {
        size_t wrapped = (CurrentCacheDataPointer + (CacheSize - 1)) % CacheSize;
        Timesteps->Set(Timesteps->FloatCache[wrapped] + getTimestep());
        Timesteps->SetValue(CurrentCacheDataPointer);
    }

    // set normal data
    for (auto tmp : Stats) {
        auto casted = dynamic_cast<CachedStatValue *>(tmp);
        if (casted != nullptr) {
            casted->SetValue(CurrentCacheDataPointer);
        }
    }

    // increase pointers and size
    if (CurrentCacheDataSize < CacheSize) {
        CurrentCacheDataSize++;
    }
    CurrentCacheDataPointer = (CurrentCacheDataPointer + 1) % CacheSize;

}

size_t FluidSolver::CachedStatisticCollector::getCurrentCacheDataSize() const {
    return CurrentCacheDataSize;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getTimestepCache() const {
    return Timesteps->FloatCache;
}

void FluidSolver::CachedStatisticCollector::RemapStatValues() {
    calculatedAverageDensity = new CachedStatValue(calculatedAverageDensity);
    calculatedEnergy = new CachedStatValue(calculatedEnergy);
    calculatedMaximumVelocity = new CachedStatValue(calculatedMaximumVelocity);
    calculatedDeadParticleCount = new CachedStatValue(calculatedDeadParticleCount);
    calculatedKineticEnergy = new CachedStatValue(calculatedKineticEnergy);
    calculatedPotentialEnergy = new CachedStatValue(calculatedPotentialEnergy);
    calculatedBoundaryParticleCount = new CachedStatValue(calculatedBoundaryParticleCount);
    calculatedNormalParticleCount = new CachedStatValue(calculatedNormalParticleCount);
    calculatedCFLNumber = new CachedStatValue(calculatedCFLNumber);
    diagonalElement = new CachedStatValue(diagonalElement);
    iterationCount = new CachedStatValue(iterationCount);
}

FluidSolver::CachedStatisticCollector::~CachedStatisticCollector() {
    delete Timesteps;
}
