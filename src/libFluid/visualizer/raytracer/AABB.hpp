#pragma once

#include "visualizer/raytracer/Ray.hpp"

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {
    struct AABB {
        glm::vec3 minimum;
        glm::vec3 maximum;


        void extend_volume_by_point(const glm::vec3& point);

        bool is_ray_intersecting(const Ray& ray) const;
    };
} // namespace LibFluid::Raytracer