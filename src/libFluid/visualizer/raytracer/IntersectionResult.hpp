#pragma once

#include <glm/glm.hpp>

namespace FluidSolver::Raytracer {


    class IntersectionResult {
      public:
        enum class IntersectionResultType {
            FluidOutside,
            FluidInside,
            BoundaryOutside,
            BoundaryInside
        } intersection_result_type;

        glm::vec3 normal_at_intersection;
        glm::vec3 point_of_intersection;
    };
} // namespace FluidSolver::Raytracer