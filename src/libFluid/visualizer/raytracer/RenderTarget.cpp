#include "RenderTarget.hpp"

#include "LibFluidAssert.hpp"

namespace LibFluid::Raytracer {


    void RenderTarget::clear() {
        for (size_t i = 0; i < data.size(); i++) {
            data[i] = glm::vec3(0.0f);
        }
    }
    void RenderTarget::set_size(size_t width, size_t height) {
        this->width = width;
        this->height = height;
        data.resize(width * height, glm::vec3(0.0f));
    }
    size_t RenderTarget::get_width() const {
        return width;
    }
    size_t RenderTarget::get_height() const {
        return height;
    }
    glm::vec3 RenderTarget::get(size_t x, size_t y) const {
        auto array_index = get_array_index_by_position(x, y);
        FLUID_ASSERT(array_index < data.size());

        return data[array_index];
    }
    void RenderTarget::set(size_t x, size_t y, const glm::vec3& radiance) {
        auto array_index = get_array_index_by_position(x, y);
        FLUID_ASSERT(array_index < data.size());

        data[array_index] = radiance;
    }
    size_t RenderTarget::get_array_index_by_position(size_t x, size_t y) const {
        if constexpr (row_major_order) {
            return y * width + x;
        } else {
            return x * height + y;
        }
    }

    HdrImage RenderTarget::as_hdr_image() const {
        if constexpr (row_major_order) {
            return HdrImage(width, height, data);
        } else {
            FLUID_ASSERT(false, "Not implemented");
            return {};
        }
    }
} // namespace LibFluid::Raytracer