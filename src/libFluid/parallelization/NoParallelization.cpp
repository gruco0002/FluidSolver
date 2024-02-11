#include "NoParallelization.hpp"

#include <algorithm>
#include <limits>

void LibFluid::NoParallelization::loop_for(size_t from, size_t to, const std::function<void(size_t i)> &fn)
{
    for (size_t i = from; i < to; i++)
    {
        fn(i);
    }
}

void LibFluid::NoParallelization::loop_for(size_t from, size_t to, size_t step, const std::function<void(size_t i)> &fn)
{
    for (size_t i = from; i < to; i += step)
    {
        fn(i);
    }
}
float LibFluid::NoParallelization::loop_for_max(size_t from, size_t to, const std::function<float(size_t)> &fn)
{
    float result = std::numeric_limits<float>::lowest();
    for (size_t i = from; i < to; i++)
    {
        result = std::max(result, fn(i));
    }
    return result;
}
