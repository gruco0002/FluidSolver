#include "AABB.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

    bool AABB::is_ray_intersecting(const Ray& ray) const {
        // TODO: implement
        return false;
    }

    void AABB::extend_volume_by_point(const glm::vec3& point) {
        minimum.x = LibFluid::Math::min(minimum.x, point.x);
        minimum.y = LibFluid::Math::min(minimum.y, point.y);
        minimum.z = LibFluid::Math::min(minimum.z, point.z);

        maximum.x = LibFluid::Math::max(maximum.x, point.x);
        maximum.y = LibFluid::Math::max(maximum.y, point.y);
        maximum.z = LibFluid::Math::max(maximum.z, point.z);
    }
} // namespace LibFluid::Raytracer