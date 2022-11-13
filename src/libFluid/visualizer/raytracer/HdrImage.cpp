#include "HdrImage.hpp"

#include "LibFluidAssert.hpp"

#include <stb_image.h>
#include <stb_image_write.h>
#include <utility>

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

    HdrImage::HdrImage(size_t width, size_t height) {
        m_width = width;
        m_height = height;
    }

    HdrImage::HdrImage(size_t width, size_t height, std::vector<LightValue> data) {
        FLUID_ASSERT(data.size() == width * height);
        m_width = width;
        m_height = height;
        m_data = std::move(data);
    }

    HdrImage::HdrImage(const std::string& filepath) {
        load_from_hdr(filepath);
    }

    HdrImage::HdrImage() = default;

    void HdrImage::save_as_hdr(const std::string& filepath) const {
        stbi_write_hdr(filepath.c_str(), m_width, m_height, 3, reinterpret_cast<const float*>(m_data.data()));
    }

    bool HdrImage::load_from_hdr(const std::string& filepath) {
        int width = 0;
        int height = 0;
        int channels = 0;
        auto data = stbi_loadf(filepath.c_str(), &width, &height, &channels, 3);

        if (data == nullptr) {
            return false;
        }

        m_width = width;
        m_height = height;
        m_data.resize(width * height);

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                size_t i = y * width + x;

                float r = data[i * 3 + 0];
                float g = data[i * 3 + 1];
                float b = data[i * 3 + 2];

                m_data[i] = LightValue(r, g, b);
            }
        }

        // free the data
        free(data);

        return true;
    }


} // namespace LibFluid::Raytracer