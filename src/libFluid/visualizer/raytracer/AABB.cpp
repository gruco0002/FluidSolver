#include "AABB.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

    bool AABB::is_ray_intersecting(const Ray& ray) const {
        // based on the 'slab method'
        // see: https://tavianator.com/2011/ray_box.html
        float tx_1 = (minimum.x - ray.starting_point.x) / ray.normalized_direction.x;
        float tx_2 = (maximum.x - ray.starting_point.x) / ray.normalized_direction.x;
        float tx_min = Math::min(tx_1, tx_2);
        float tx_max = Math::max(tx_1, tx_2);

        float ty_1 = (minimum.y - ray.starting_point.y) / ray.normalized_direction.y;
        float ty_2 = (maximum.y - ray.starting_point.y) / ray.normalized_direction.y;
        float ty_min = Math::min(ty_1, ty_2);
        float ty_max = Math::max(ty_1, ty_2);

        if (tx_min > ty_max || ty_min > tx_max) {
            return false;
        }

        float t_min = Math::max(tx_min, ty_min);
        float t_max = Math::min(tx_max, ty_max);

        float tz_1 = (minimum.z - ray.starting_point.z) / ray.normalized_direction.z;
        float tz_2 = (maximum.z - ray.starting_point.z) / ray.normalized_direction.z;
        float tz_min = Math::min(tz_1, tz_2);
        float tz_max = Math::max(tz_1, tz_2);

        if (t_min > tz_max || tz_min > t_max) {
            return false;
        }

        return true;
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