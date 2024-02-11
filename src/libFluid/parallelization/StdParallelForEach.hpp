#pragma once

#include <functional>

namespace LibFluid
{
    class StdParallelForEach
    {

      public:
        static constexpr size_t chunk_size = 16;

        static void loop_for(size_t from, size_t to, const std::function<void(size_t i)> &fn);

        static float loop_for_max(size_t from, size_t to, const std::function<float(size_t i)> &fn);

        static void loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)> &fn);
    };

} // namespace LibFluid
