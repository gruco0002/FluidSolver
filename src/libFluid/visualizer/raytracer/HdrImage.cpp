#include "HdrImage.hpp"

#include "LibFluidAssert.hpp"

namespace LibFluid::Raytracer {


    size_t HdrImage::width() const {
        return m_width;
    }
    size_t HdrImage::height() const {
        return m_height;
    }
    size_t HdrImage::size() const {
        return m_data.size();
    }
    LightValue* HdrImage::data() {
        return &m_data[0];
    }
    const LightValue* HdrImage::data() const {
        return &m_data[0];
    }

    void HdrImage::set(size_t x, size_t y, const LightValue& color) {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        m_data[y * m_width + x] = color;
    }

    LightValue& HdrImage::get(size_t x, size_t y) {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        return m_data[y * m_width + x];
    }

    const LightValue& HdrImage::get(size_t x, size_t y) const {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        return m_data[y * m_width + x];
    }
} // namespace LibFluid::Raytracer