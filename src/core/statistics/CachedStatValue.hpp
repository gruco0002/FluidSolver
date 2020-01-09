//
// Created by corbi on 09.01.2020.
//

#ifndef FLUIDSOLVER_CACHEDSTATVALUE_HPP
#define FLUIDSOLVER_CACHEDSTATVALUE_HPP

#include <vector>
#include "StatValue.hpp"

namespace FluidSolver {
    class CachedStatValue : public StatValue {
    public:
        explicit CachedStatValue(StatValue *statValue);

        CachedStatValue(const std::string &name, const std::string &description, StatValueType valueType);

        std::vector<float> FloatCache;
        std::vector<uint32_t> UIntCache;

        void Resize(size_t newCacheSize);

        size_t GetSize();

        void SetValue(size_t i);


    };

}


#endif //FLUIDSOLVER_CACHEDSTATVALUE_HPP
