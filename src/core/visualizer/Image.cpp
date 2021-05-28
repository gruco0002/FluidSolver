#include "Image.hpp"

#include "core/FluidInclude.hpp"
#include "stb_image_write.h"

FluidSolver::Image::Color::Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b), a(255)
{
}

FluidSolver::Image::Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a)
{
}

FluidSolver::Image::Color::Color(const glm::vec3& color)
{
    r = (unsigned char)(color.r * 255.0f);
    g = (unsigned char)(color.g * 255.0f);
    b = (unsigned char)(color.b * 255.0f);
    a = 255;
}

FluidSolver::Image::Color::Color(const glm::vec4& color)
{
    r = (unsigned char)(color.r * 255.0f);
    g = (unsigned char)(color.g * 255.0f);
    b = (unsigned char)(color.b * 255.0f);
    a = (unsigned char)(color.a * 255.0f);
}

FluidSolver::Image::Image(size_t width, size_t height) : v_width(width), v_height(height)
{
    pixels.resize(width * height, Color());
}

size_t FluidSolver::Image::width() const
{
    return v_width;
}

size_t FluidSolver::Image::height() const
{
    return v_height;
}

size_t FluidSolver::Image::size() const
{
    return v_width * v_height;
}

FluidSolver::Image::Color* FluidSolver::Image::data()
{
    return pixels.data();
}

const FluidSolver::Image::Color* FluidSolver::Image::data() const
{
    return pixels.data();
}

void FluidSolver::Image::set(size_t x, size_t y, const Color& color)
{
    FLUID_ASSERT(x < v_width);
    FLUID_ASSERT(y < v_height);

    pixels[y * v_width + x] = color;
}

FluidSolver::Image::Color& FluidSolver::Image::get(size_t x, size_t y)
{
    FLUID_ASSERT(x < v_width);
    FLUID_ASSERT(y < v_height);

    return pixels[y * v_width + x];
}

void FluidSolver::Image::save_as_png(const std::string& filepath) const
{
    stbi_write_png(filepath.c_str(), v_width, v_height, 4, pixels.data(), v_width * 4);
}
