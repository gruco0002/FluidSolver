#pragma once

#include "visualizer/raytracer/IntersectionResult.hpp"
#include "visualizer/raytracer/Ray.hpp"

#include <array>
#include <random>

namespace LibFluid::Raytracer {
    class Sampler {
      public:
        Ray sample_cosine_weighted_hemisphere(const glm::vec3& normalized_up);

        float get_uniform_sampled_value();

      private:

        struct RandomGenerator{
            std::default_random_engine random_engine;
            std::uniform_real_distribution<float> uniform_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

            float generate();
        };

        std::array<RandomGenerator, 3> generators {};


    };
} // namespace LibFluid::Raytracer