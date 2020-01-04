//
// Created by corbi on 05.01.2020.
//

#ifndef FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP

#include <vector>
#include "StatisticCollector.hpp"

namespace FluidSolver {
    class CachedStatisticCollector : public StatisticCollector {

    public:

        CachedStatisticCollector();

        void ResetCache();

        void CalculateData() override;


    private:
        size_t CacheSize = 100;

        size_t CurrentCacheDataSize = 0;
        size_t CurrentCacheDataPointer = 0;

        std::vector<float> TimestepCache;
        std::vector<float> AverageDensityCache;
        std::vector<float> EnergyCache;
        std::vector<float> MaximumVelocityCache;
        std::vector<uint32_t> DeadParticleCountCache;
        std::vector<float> KineticEnergyCache;
        std::vector<float> PotentialEnergyCache;
        std::vector<uint32_t> BoundaryParticleCountCache;
        std::vector<uint32_t> NormalParticleCountCache;
        std::vector<float> CflNumberCache;

        void ResizeVectors();

        void CreateNewCacheEntry();

    public:
        size_t getCurrentCacheDataSize() const;

        const std::vector<float> &getTimestepCache() const;

        const std::vector<float> &getAverageDensityCache() const;

        const std::vector<float> &getEnergyCache() const;

        const std::vector<float> &getMaximumVelocityCache() const;

        const std::vector<uint32_t> &getDeadParticleCountCache() const;

        const std::vector<float> &getKineticEnergyCache() const;

        const std::vector<float> &getPotentialEnergyCache() const;

        const std::vector<uint32_t> &getBoundaryParticleCountCache() const;

        const std::vector<uint32_t> &getNormalParticleCountCache() const;

        const std::vector<float> &getCflNumberCache() const;

    };
}


#endif //FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP
