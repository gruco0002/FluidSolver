#pragma once

namespace LibFluid::Math {
    constexpr float PI = 3.14159265358979323846f;

    // defines the kernel radius / support w.r.t. particle size
    constexpr float kernel_support_factor = 2.0f;

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
    inline float abs(const float& a) {
        return std::abs(a);
    }

    inline glm::vec3 safe_normalize(const glm::vec3& v) {
        auto result = glm::normalize(v);
        if (glm::any(glm::isnan(result))) {
            return glm::vec3(0.0f);
        }
        return result;
    }
} // namespace LibFluid::Math