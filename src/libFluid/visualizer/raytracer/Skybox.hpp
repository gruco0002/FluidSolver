#pragma once

#include "HdrImage.hpp"
#include "RenderTarget.hpp"

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {

    class Skybox {
      public:
        LightValue get_light_value_by_direction(const glm::vec3& normalized_direction) const;

        HdrImage skybox_image;

      private:
        glm::vec2 direction_to_pixel_coordinate(const glm::vec3& normalized_direction) const;

        LightValue get_interpolated_color(const glm::vec2& position) const;
    };


} // namespace LibFluid::Raytracer