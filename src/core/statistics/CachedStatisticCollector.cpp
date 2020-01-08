//
// Created by corbi on 05.01.2020.
//

#include "CachedStatisticCollector.hpp"

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
    ResizeVectors();
}

void FluidSolver::CachedStatisticCollector::ResizeVectors() {
    TimestepCache.resize(CacheSize);
    AverageDensityCache.resize(CacheSize);
    EnergyCache.resize(CacheSize);
    MaximumVelocityCache.resize(CacheSize);
    DeadParticleCountCache.resize(CacheSize);
    KineticEnergyCache.resize(CacheSize);
    PotentialEnergyCache.resize(CacheSize);
    BoundaryParticleCountCache.resize(CacheSize);
    NormalParticleCountCache.resize(CacheSize);
    CflNumberCache.resize(CacheSize);
}

void FluidSolver::CachedStatisticCollector::CreateNewCacheEntry() {

    // timestep is added up if possible
    if (CurrentCacheDataSize == 0) {
        TimestepCache[CurrentCacheDataPointer] = getTimestep();
    } else {
        size_t wrapped = (CurrentCacheDataPointer + (CacheSize - 1)) % CacheSize;
        TimestepCache[CurrentCacheDataPointer] = TimestepCache[wrapped] + getTimestep();
    }

    // set normal data
    AverageDensityCache[CurrentCacheDataPointer] = getCalculatedAverageDensity();
    EnergyCache[CurrentCacheDataPointer] = getCalculatedEnergy();
    MaximumVelocityCache[CurrentCacheDataPointer] = getCalculatedMaximumVelocity();
    DeadParticleCountCache[CurrentCacheDataPointer] = getCalculatedDeadParticleCount();
    KineticEnergyCache[CurrentCacheDataPointer] = getCalculatedKineticEnergy();
    PotentialEnergyCache[CurrentCacheDataPointer] = getCalculatedPotentialEnergy();
    BoundaryParticleCountCache[CurrentCacheDataPointer] = getCalculatedBoundaryParticleCount();
    NormalParticleCountCache[CurrentCacheDataPointer] = getCalculatedNormalParticleCount();
    CflNumberCache[CurrentCacheDataPointer] = getCalculatedCflNumber();

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
    return TimestepCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getAverageDensityCache() const {
    return AverageDensityCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getEnergyCache() const {
    return EnergyCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getMaximumVelocityCache() const {
    return MaximumVelocityCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getDeadParticleCountCache() const {
    return DeadParticleCountCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getKineticEnergyCache() const {
    return KineticEnergyCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getPotentialEnergyCache() const {
    return PotentialEnergyCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getBoundaryParticleCountCache() const {
    return BoundaryParticleCountCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getNormalParticleCountCache() const {
    return NormalParticleCountCache;
}

const std::vector<float> &FluidSolver::CachedStatisticCollector::getCflNumberCache() const {
    return CflNumberCache;
}
