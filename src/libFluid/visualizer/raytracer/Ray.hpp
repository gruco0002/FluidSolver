#pragma once

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {

    class Ray {
      public:
        glm::vec3 starting_point;
        glm::vec3 normalized_direction;

        float solid_angle;

        float length_until_hit = -1;

        glm::vec3 get_hit_location() const;
    };
} // namespace FluidSolver::Raytracer