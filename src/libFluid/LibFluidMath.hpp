#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <limits>

namespace LibFluid::Math
{
    constexpr float PI = 3.14159265358979323846f;

    // defines the kernel radius / support w.r.t. particle size
    constexpr float kernel_support_factor = 2.0f;

    inline float pow2(const float &x)
    {
        return x * x;
    }

    inline float pow3(const float &x)
    {
        return x * x * x;
    }
    inline float pow4(const float &x)
    {
        return x * x * x * x;
    }
    inline float max(const float &a, const float &b)
    {
        return std::fmax(a, b);
    }
    inline float min(const float &a, const float &b)
    {
        return std::fmin(a, b);
    }
    inline float sqrt(const float &a)
    {
        return std::sqrt(a);
    }
    inline float abs(const float &a)
    {
        return std::abs(a);
    }

    inline glm::vec3 safe_normalize(const glm::vec3 &v)
    {
        auto result = glm::normalize(v);
        if (glm::any(glm::isnan(result)))
        {
            return glm::vec3(0.0f);
        }
        return result;
    }

    inline bool is_not_zero(const float &f)
    {
        return abs(f) > std::numeric_limits<float>::epsilon();
    }

    inline bool is_zero(const float &f)
    {
        return abs(f) <= std::numeric_limits<float>::epsilon();
    }

    inline bool is_zero(const glm::vec3 &v)
    {
        return is_zero(v.x) && is_zero(v.y) && is_zero(v.z);
    }
} // namespace LibFluid::Math
