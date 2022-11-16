#include "CosineWeightedHemisphereDistribution.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer::Distributions::CosineWeightedHemisphereDistribution {

    Ray sample_ray(const glm::vec2& uniform_dist_samples, const glm::vec3& normalized_up) {
        Ray result;

        float zeta_one = uniform_dist_samples.x;
        float zeta_two = uniform_dist_samples.y;

        // calculate the sampled direction
        {
            // calculate the sampled direction from the random values
            float x = cosf(2.0f * Math::PI * zeta_two) * sqrtf(zeta_one);
            float y = sinf(2.0f * Math::PI * zeta_two) * sqrtf(zeta_one);
            float z = sqrtf(1.0f - zeta_one);

            // the created vector has length one due to the calculations above
            glm::vec3 normalized_sampled_direction(x, y, z);

            // the sampled direction is in a hemisphere, that sits on the x-y plane, pointing in the z direction
            // now we transform the sampled direction into the hemisphere space defined by
            // the up vector (normalized_up), which means that we rotate it accordingly

            // start by determining an arbitrary tangent
            glm::vec3 tangent;
            if (Math::is_not_zero(normalized_up.x)) {
                tangent.x = (-normalized_up.y - normalized_up.z) / normalized_up.x;
                tangent.y = 1.0f;
                tangent.z = 1.0f;
            } else if (Math::is_not_zero(normalized_up.y)) {
                tangent.x = 1.0f;
                tangent.y = (-normalized_up.x - normalized_up.z) / normalized_up.y;
                tangent.z = 1.0f;
            } else {
                // z component has to be unequal to zero
                tangent.x = 1.0f;
                tangent.y = 1.0f;
                tangent.z = (-normalized_up.x - normalized_up.y) / normalized_up.z;
            }
            tangent = glm::normalize(tangent);

            // calculate the resulting bitangent
            glm::vec3 bitangent = glm::normalize(glm::cross(normalized_up, tangent));

            // create a change-of-basis matrix, the created matrix is orthonormal
            // it follows, that the length of vectors multiplied with this matrix remains the same
            glm::mat3 change_of_basis_matrix = glm::mat3(tangent, bitangent, normalized_up);
            result.normalized_direction = change_of_basis_matrix * normalized_sampled_direction;
        }

        // calculate estimated solid angle
        {
            float theta = asinf(sqrtf(zeta_one));
            //float phi = 2.0f * Math::PI * zeta_two;
            float probability = std::cosf(theta) / Math::PI;
            float estimated_solid_angle = 1.0f / probability;
            result.solid_angle = estimated_solid_angle;
        }

        return result;
    }
} // namespace LibFluid::Raytracer::Distributions::CosineWeightedHemisphereDistribution