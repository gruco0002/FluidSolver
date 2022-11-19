#include "Ray.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {


    glm::vec3 Ray::get_hit_location() const {
        return starting_point + normalized_direction * length_until_hit;
    }

    Ray::operator bool() const {
        if (Math::is_zero(starting_point - glm::vec3(std::numeric_limits<float>::lowest()))) {
            return false; // the starting position was not initialized
        }

        if (Math::is_zero(normalized_direction)) {
            return false; // the direction was not initialized
        }
        return true;
    }
} // namespace LibFluid::Raytracer