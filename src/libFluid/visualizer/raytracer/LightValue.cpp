#include "LightValue.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

    void LightValue::add(const LightValue& other) {
        r += other.r;
        g += other.g;
        b += other.b;
    }
    void LightValue::mul(float factor) {
        r *= factor;
        g *= factor;
        b *= factor;
    }
    void LightValue::mul(const LightValue& other) {
        r *= other.r;
        g *= other.g;
        b *= other.b;
    }
    bool LightValue::is_zero() const {
        return Math::is_zero(r) && Math::is_zero(g) && Math::is_zero(b);
    }
    LightValue::LightValue() {
        r = 0.0f;
        g = 0.0f;
        b = 0.0f;
    }
    LightValue::LightValue(float value) {
        r = value;
        g = value;
        b = value;
    }
    LightValue::LightValue(float r, float g, float b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    LightValue::LightValue(const glm::vec3& value) {
        r = value.x;
        g = value.y;
        b = value.z;
    }
} // namespace LibFluid::Raytracer