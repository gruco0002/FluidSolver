#pragma once

#include "visualizer/raytracer/LightValue.hpp"
#include "visualizer/raytracer/HdrImage.hpp"

#include <glm/glm.hpp>
#include <vector>


namespace LibFluid::Raytracer {


    class RenderTarget {
      public:
        void clear();
        void set_size(size_t width, size_t height);

        size_t get_width() const;
        size_t get_height() const;

        LightValue get(size_t x, size_t y) const;
        void set(size_t x, size_t y, const LightValue& light_value);

        HdrImage as_hdr_image() const;


      private:
        static constexpr bool row_major_order = true;

        size_t width;
        size_t height;
        std::vector<LightValue> data;

        size_t get_array_index_by_position(size_t x, size_t y) const;
    };


} // namespace LibFluid::Raytracer