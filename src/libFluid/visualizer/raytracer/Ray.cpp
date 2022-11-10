#include "Ray.hpp"
namespace LibFluid::Raytracer {


    glm::vec3 Ray::get_hit_location() const {
        return starting_point + normalized_direction * length_until_hit;
    }
}