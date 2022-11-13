#pragma once

#include "visualizer/raytracer/LightValue.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace LibFluid::Raytracer {

    class HdrImage {
      public:
        HdrImage(size_t width, size_t height);
        HdrImage(size_t width, size_t height, std::vector<LightValue> data);
        HdrImage();
        explicit HdrImage(const std::string& filepath);

        size_t width() const;
        size_t height() const;
        size_t size() const;
        LightValue* data();
        const LightValue* data() const;

        void set(size_t x, size_t y, const LightValue& color);
        LightValue& get(size_t x, size_t y);
        const LightValue& get(size_t x, size_t y) const;

        void save_as_hdr(const std::string& filepath) const;
        bool load_from_hdr(const std::string& filepath);

      private:
        std::vector<LightValue> m_data;
        size_t m_width = 0;
        size_t m_height = 0;
    };

} // namespace LibFluid::Raytracer
