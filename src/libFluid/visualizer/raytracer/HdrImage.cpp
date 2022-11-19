#include "HdrImage.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

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
    glm::vec3* HdrImage::data() {
        return &m_data[0];
    }
    const glm::vec3* HdrImage::data() const {
        return &m_data[0];
    }

    void HdrImage::set(size_t x, size_t y, const glm::vec3& color) {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        m_data[y * m_width + x] = color;
    }

    glm::vec3& HdrImage::get(size_t x, size_t y) {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        return m_data[y * m_width + x];
    }

    const glm::vec3& HdrImage::get(size_t x, size_t y) const {
        FLUID_ASSERT(x < m_width);
        FLUID_ASSERT(y < m_height);
        return m_data[y * m_width + x];
    }

    HdrImage::HdrImage(size_t width, size_t height) {
        m_width = width;
        m_height = height;
        m_data.resize(width * height);
    }

    HdrImage::HdrImage(size_t width, size_t height, std::vector<glm::vec3> data) {
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

        process_hdr_file_data(data, width, height);

        // free the data
        free(data);

        return true;
    }

    void get_as_hdr_helper(void* context, void* data, int size) {
        // reinterpret the value back as vector
        auto& result = *reinterpret_cast<std::vector<uint8_t>*>(context);

        // add the new data to the vector
        size_t current_size = result.size();
        result.resize(current_size + size);
        std::memcpy(&result[current_size], data, size);
    }


    std::vector<uint8_t> HdrImage::get_as_hdr_file_data() const {
        std::vector<uint8_t> result;
        stbi_write_hdr_to_func(get_as_hdr_helper, &result, m_width, m_height, 3, reinterpret_cast<const float*>(m_data.data()));
        return result;
    }

    bool HdrImage::load_from_hdr_file_data(const std::vector<uint8_t>& data) {
        int width = 0;
        int height = 0;
        int channels = 0;
        auto result = stbi_loadf_from_memory(data.data(), data.size(), &width, &height, &channels, 3);

        if (result == nullptr) {
            return false;
        }

        process_hdr_file_data(result, width, height);

        // free the created data
        free(result);

        return false;
    }

    void HdrImage::process_hdr_file_data(const float* data, int width, int height) {
        m_width = width;
        m_height = height;
        m_data.resize(width * height);

        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                size_t i = y * width + x;

                float r = data[i * 3 + 0];
                float g = data[i * 3 + 1];
                float b = data[i * 3 + 2];

                m_data[i] = glm::vec3(r, g, b);
            }
        }
    }

    HdrImage HdrImage::apply_box_blur(size_t size) const {
        // TODO: improve performance by separating the filter into two passes
        HdrImage result(width(), height());

        auto get_value_safe = [](const HdrImage& input, int x, int y) {
            if (x < 0)
                x = -x;
            if (y < 0)
                y = -y;
            if (x >= input.width())
                x = input.width() - (x - input.width() + 1);
            if (y >= input.height())
                y = input.height() - (y - input.height() + 1);

            FLUID_ASSERT(x >= 0 && x < input.width());
            FLUID_ASSERT(y >= 0 && y < input.height());
            return input.get(x, y);
        };

        for (size_t y = 0; y < height(); y++) {
            for (size_t x = 0; x < width(); x++) {
                glm::vec3 pixel(0.0f);

                for (int v = y - size; v <= y + size; v++) {
                    for (int u = x - size; u <= x + size; u++) {
                        pixel += get_value_safe(*this, u, v);
                    }
                }

                pixel *= 1.0f / Math::pow2(2.0f * static_cast<float>(size) + 1.0f);
                result.set(x, y, pixel);
            }
        }

        return result;
    }


} // namespace LibFluid::Raytracer