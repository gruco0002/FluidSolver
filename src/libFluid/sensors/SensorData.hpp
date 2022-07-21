#pragma once

#include "time/Timepoint.hpp"

#include <vector>

namespace LibFluid {

    template <typename T> class SensorData {

      private:
        std::vector<T> values;
        std::vector<Timepoint> timepoints;

      public:
        const T* data() const
        {
            FLUID_ASSERT(values.size() == timepoints.size());
            return values.data();
        }

        const Timepoint* times() const
        {
            FLUID_ASSERT(values.size() == timepoints.size());
            return timepoints.data();
        }

        void push_back(const Timepoint& time, const T& value)
        {
            timepoints.push_back(time);
            values.push_back(value);
        }

        void clear()
        {
            timepoints.clear();
            values.clear();
        }

        size_t size() const
        {
            FLUID_ASSERT(values.size() == timepoints.size());
            return values.size();
        }
    };

} // namespace FluidSolver