#pragma once

namespace LibFluid::Math {
    constexpr float PI = 3.14159265358979323846f;

    inline float pow2(const float& x) {
        return x * x;
    }

    inline float pow3(const float& x) {
        return x * x * x;
    }
    inline float pow4(const float& x) {
        return x * x * x * x;
    }
    inline float max(const float& a, const float& b) {
        return std::fmax(a, b);
    }
    inline float min(const float& a, const float& b) {
        return std::fmin(a, b);
    }
    inline float sqrt(const float& a) {
        return std::sqrt(a);
    }
} // namespace FluidSolver::Math