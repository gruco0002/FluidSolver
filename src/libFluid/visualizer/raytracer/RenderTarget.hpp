#pragma once

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

        glm::vec3 get(size_t x, size_t y) const;
        void set(size_t x, size_t y, const glm::vec3& light_value);

        HdrImage as_hdr_image() const;


      private:
        static constexpr bool row_major_order = true;

        size_t width;
        size_t height;
        std::vector<glm::vec3> data;

        size_t get_array_index_by_position(size_t x, size_t y) const;
    };


} // namespace LibFluid::Raytracer