#include "ParticleIntersectionAccelerator.hpp"

#include "LibFluidAssert.hpp"

namespace LibFluid::Raytracer {

    void ParticleIntersectionAccelerator::prepare() {
        // calculate aabb
        calculate_aabb();
    }

    bool ParticleIntersectionAccelerator::is_intersecting_with_particles(Ray& ray, IntersectionResult& result) {
        if (!particle_surrounding_aabb.is_ray_intersecting(ray)) {
            // the ray does not intersect the aabb around the particles
            return false;
        }

        constexpr  size_t max_steps = 1000000;

        float step_size = particle_size / 2.0f;

        const auto initial_result = evaluate_volume_at_position(ray.starting_point + step_size / 2.0f * ray.normalized_direction);

        for(size_t t = 1; t < max_steps; t++){

            auto position = ray.starting_point + step_size * (float)t * ray.normalized_direction;

            auto result = evaluate_volume_at_position(position);

            // TODO: implement


        }


        return false;
    }

    void ParticleIntersectionAccelerator::calculate_aabb() {
        FLUID_ASSERT(particle_collection != nullptr);
        FLUID_ASSERT(particle_collection->is_type_present<MovementData3D>());


        bool first = true;
        for (size_t i = 0; i < particle_collection->size(); i++) {
            const auto& mv = particle_collection->get<MovementData3D>(i);
            const auto& pi = particle_collection->get<ParticleInfo>(i);
            if (pi.type == ParticleType::ParticleTypeInactive) {
                continue;
            }

            if (first) {
                first = false;
                particle_surrounding_aabb.maximum = mv.position;
                particle_surrounding_aabb.minimum = mv.position;
            }

            particle_surrounding_aabb.extend_volume_by_point(mv.position);
        }
    }
} // namespace LibFluid::Raytracer