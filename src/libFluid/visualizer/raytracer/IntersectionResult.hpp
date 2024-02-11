#pragma once

#include <glm/glm.hpp>

namespace LibFluid::Raytracer
{

    class IntersectionResult
    {
      public:
        enum class IntersectionResultType
        {
            RayReachedFluidSurfaceFromOutsideTheFluid,
            RayReachedFluidSurfaceFromInsideTheFluid,
            RayHitBoundarySurface
        } intersection_result_type;

        glm::vec3 normal_at_intersection;
        glm::vec3 point_of_intersection;
    };
} // namespace LibFluid::Raytracer
