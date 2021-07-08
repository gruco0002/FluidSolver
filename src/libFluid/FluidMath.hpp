#pragma once

namespace FluidSolver::Math
{
    constexpr float PI = 3.14159265358979323846f;

    inline float pow2(const float& x)
    {
        return x * x;
    }

    inline float pow3(const float& x)
    {
        return x * x * x;
    }
    inline float pow4(const float& x)
    {
        return x * x * x * x;
    }
} // namespace FluidSolver::Math