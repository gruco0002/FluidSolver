#pragma once

#include <glm/glm.hpp>



namespace LibFluid::Raytracer {
    class Sampler {
      public:
        glm::vec2 get_uniform_sampled_pair();

        float get_uniform_sampled_value();

    };
} // namespace LibFluid::Raytracer