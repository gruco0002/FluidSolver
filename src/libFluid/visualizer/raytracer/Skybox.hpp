#pragma once

#include "visualizer/raytracer/HdrImage.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"
#include "visualizer/raytracer/distributions/PiecewiseConstantDistribution2D.hpp"

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {

    class Skybox {
      public:
        HdrImage skybox_image;

        glm::vec3 get_light_value_by_direction(const glm::vec3& normalized_direction) const;

        glm::vec3 sample_normalized_direction(const glm::vec2& uniform_dist_random_sample_pair, float& pdf) const;

        void prepare();

        bool has_data() const;

      private:
        Distributions::PiecewiseConstantDistribution2D skybox_image_distribution;

        glm::vec2 direction_to_pixel_coordinate(const glm::vec3& normalized_direction) const;

        glm::vec3 get_interpolated_color(const glm::vec2& position) const;
    };


} // namespace LibFluid::Raytracer