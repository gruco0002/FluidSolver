#pragma once

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {

    struct LightValue {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;

        void add(const LightValue& other);
        void mul(float factor);
        void mul(const LightValue& other);
        bool is_zero() const;

        LightValue();
        explicit LightValue(float value);
        LightValue(float r, float g, float b);
        explicit LightValue(const glm::vec3& value);
    };

} // namespace LibFluid::Raytracer