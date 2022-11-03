#include "RenderTarget.hpp"

#include "LibFluidAssert.hpp"

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
        return r == 0.0f && g == 0.0f && b == 0.0f;
    }
    void RenderTarget::clear() {
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = {};
        }
    }
    void RenderTarget::set_size(size_t width, size_t height) {
        this->width = width;
        this->height = height;
        data.resize(width * height, {});
    }
    size_t RenderTarget::get_width() const {
        return width;
    }
    size_t RenderTarget::get_height() const {
        return height;
    }
    LightValue RenderTarget::get(size_t x, size_t y) const {
        auto array_index = get_array_index_by_position(x, y);
        FLUID_ASSERT(array_index < data.size());

        return data[array_index];
    }
    void RenderTarget::set(size_t x, size_t y, const LightValue& light_value) {
        auto array_index = get_array_index_by_position(x, y);
        FLUID_ASSERT(array_index < data.size());

        data[array_index] = light_value;
    }
    size_t RenderTarget::get_array_index_by_position(size_t x, size_t y) const {
        if constexpr (row_major_order) {
            return y * width + x;
        } else {
            return x * height + y;
        }
    }
} // namespace FluidSolver::Raytracer