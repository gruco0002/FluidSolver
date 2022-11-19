#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace LibFluid::Raytracer {

    class Ray {
      public:
        glm::vec3 starting_point = glm::vec3(std::numeric_limits<float>::lowest());
        glm::vec3 normalized_direction = glm::vec3(0.0f);

        float solid_angle = 0.0f;

        float length_until_hit = -1;

        glm::vec3 get_hit_location() const;

        operator bool() const;
    };
} // namespace LibFluid::Raytracer