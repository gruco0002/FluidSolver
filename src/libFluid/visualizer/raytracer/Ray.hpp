#pragma once

#include <glm/glm.hpp>

namespace FluidSolver::Raytracer {

    class Ray {
      public:
        glm::vec3 starting_point;
        glm::vec3 normalized_direction;

        float solid_angle;

        float length_until_hit = -1;


    };
} // namespace FluidSolver::Raytracer