#include "Image.hpp"

#include "FluidInclude.hpp"
#include <stb_image_write.h>

namespace LibFluid {


    Image::Color::Color(unsigned char r, unsigned char g, unsigned char b)
        : r(r), g(g), b(b), a(255) {
    }

    Image::Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : r(r), g(g), b(b), a(a) {
    }

    Image::Color::Color(const glm::vec3& color) {
        r = (unsigned char)(color.r * 255.0f);
        g = (unsigned char)(color.g * 255.0f);
        b = (unsigned char)(color.b * 255.0f);
        a = 255;
    }

    Image::Color::Color(const glm::vec4& color) {
        r = (unsigned char)(color.r * 255.0f);
        g = (unsigned char)(color.g * 255.0f);
        b = (unsigned char)(color.b * 255.0f);
        a = (unsigned char)(color.a * 255.0f);
    }

    Image::Image(size_t width, size_t height)
        : v_width(width), v_height(height) {
        pixels.resize(width * height, Color());
    }

    size_t Image::width() const {
        return v_width;
    }

    size_t Image::height() const {
        return v_height;
    }

    size_t Image::size() const {
        return v_width * v_height;
    }

    Image::Color* Image::data() {
        return pixels.data();
    }

    const Image::Color* Image::data() const {
        return pixels.data();
    }

    void Image::set(size_t x, size_t y, const Color& color) {
        FLUID_ASSERT(x < v_width);
        FLUID_ASSERT(y < v_height);

        pixels[y * v_width + x] = color;
    }

    Image::Color& Image::get(size_t x, size_t y) {
        FLUID_ASSERT(x < v_width);
        FLUID_ASSERT(y < v_height);

        return pixels[y * v_width + x];
    }

    void Image::save_as_png(const std::string& filepath) const {
        stbi_write_png(filepath.c_str(), v_width, v_height, 4, pixels.data(), v_width * 4);
    }

    void get_as_png_helper(void* context, void* data, int size) {
        // reinterpret the value back as vector
        auto& result = *reinterpret_cast<std::vector<uint8_t>*>(context);

        // add the new data to the vector
        size_t current_size = result.size();
        result.resize(current_size + size);
        std::memcpy(&result[current_size], data, size);
    }

    std::vector<uint8_t> Image::get_as_png() const {
        std::vector<uint8_t> result;
        stbi_write_png_to_func(get_as_png_helper, &result, v_width, v_height, 4, pixels.data(), v_width * 4);
        return result;
    }

} // namespace LibFluid