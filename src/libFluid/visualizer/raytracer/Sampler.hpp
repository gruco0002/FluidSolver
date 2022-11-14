#pragma once

#include "visualizer/raytracer/IntersectionResult.hpp"
#include "visualizer/raytracer/Ray.hpp"



namespace LibFluid::Raytracer {
    class Sampler {
      public:
        Ray sample_cosine_weighted_hemisphere(const glm::vec3& normalized_up);

        float get_uniform_sampled_value();

    };
} // namespace LibFluid::Raytracer