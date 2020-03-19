#ifndef FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP

#include <vector>
#include "StatisticCollector.hpp"
#include "CachedStatValue.hpp"

namespace FluidSolver {
    class CachedStatisticCollector : public StatisticCollector {

    public:

        CachedStatisticCollector();

        void ResetCache();

        void CalculateData() override;

        ~CachedStatisticCollector() override;


    private:
        size_t CacheSize = 1000;

        size_t CurrentCacheDataSize = 0;
        size_t CurrentCacheDataPointer = 0;

        void RemapStatValues();

        void ResizeVectors();

        void CreateNewCacheEntry();

        CachedStatValue *Timesteps = new CachedStatValue("Timesteps", "Timesteps in seconds.",
                                                         StatValue::StatValueTypeFloat);

    public:
        size_t getCurrentCacheDataSize() const;

        const std::vector<float> &getTimestepCache() const;

    };
}


#endif //FLUIDSOLVER_CACHEDSTATISTICCOLLECTOR_HPP
