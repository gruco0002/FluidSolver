#include "ParticleIntersectionAccelerator.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

namespace LibFluid::Raytracer {

    void ParticleIntersectionAccelerator::prepare() {
        // calculate aabb
        calculate_aabb();

        kernel.kernel_support = Math::kernel_support_factor * particle_size;
        kernel.initialize();

        neighborhood_search.collection = particle_collection;
        neighborhood_search.search_radius = Math::kernel_support_factor * particle_size;
        neighborhood_search.initialize();
        neighborhood_search.find_neighbors();
    }

    bool ParticleIntersectionAccelerator::is_intersecting_with_particles(Ray& ray, IntersectionResult& result) {
        if (!particle_surrounding_aabb.is_ray_intersecting(ray)) {
            // the ray does not intersect the aabb around the particles
            return false;
        }

        FLUID_ASSERT(sampler != nullptr);

        constexpr size_t max_steps = 1000000;
        float step_size = particle_size / 2.0f;
        float offset = sampler->get_uniform_sampled_value() * step_size;

        bool ray_was_within_aabb = false;

        auto last = evaluate_volume_at_position(ray.starting_point + step_size / 2.0f * ray.normalized_direction);
        for (size_t t = 1; t < max_steps; t++) {
            auto position = ray.starting_point + (step_size * (float)t + offset) * ray.normalized_direction;

            if (!particle_surrounding_aabb.is_point_within(position)) {
                if (ray_was_within_aabb) {
                    // the ray already entered the aabb before
                    // this means that is has now left the aabb and will never enter it again
                    // hence we do not need to do further checks here
                    break;
                }

                // for points outside we cannot evaluate a volume
                continue;
            }
            ray_was_within_aabb = true;

            auto current = evaluate_volume_at_position(position);

            // check if something changed compared to the last evaluation
            if (is_surface_intersected(last, current, result)) {
                ray.length_until_hit = step_size * (float)t;
                return true;
            }

            // update the last evaluation
            last = current;
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

        particle_surrounding_aabb = particle_surrounding_aabb.extended_in_all_directions_by(particle_size * Math::kernel_support_factor);
    }

    ParticleIntersectionAccelerator::VolumeEvaluationResult ParticleIntersectionAccelerator::evaluate_volume_at_position(const glm::vec3& position) {
        VolumeEvaluationResult result;
        result.position = position;

        auto neighbors = neighborhood_search.get_neighbors(position);

        for (auto index : neighbors) {
            const auto& mv = particle_collection->get<MovementData3D>(index);
            const auto& pd = particle_collection->get<ParticleData>(index);
            const auto& pi = particle_collection->get<ParticleInfo>(index);

            if (pi.type == ParticleType::ParticleTypeInactive) {
                continue;
            }

            float contribution = pd.mass * kernel.GetKernelValue(mv.position, position);
            auto normal_contribution = pd.mass * kernel.GetKernelDerivativeValue(mv.position, position);

            result.density += contribution;

            if (pi.type == ParticleType::ParticleTypeNormal) {
                result.fluid_only_density += contribution;
                result.fluid_only_normal += normal_contribution;

            } else if (pi.type == ParticleType::ParticleTypeBoundary) {
                result.boundary_only_density += contribution;
                result.boundary_only_normal += normal_contribution;
            }
        }

        return result;
    }

    bool ParticleIntersectionAccelerator::is_surface_intersected(const ParticleIntersectionAccelerator::VolumeEvaluationResult& last, const ParticleIntersectionAccelerator::VolumeEvaluationResult& current, IntersectionResult& result) {
        auto last_volume_state = get_volume_state(last);
        auto current_volume_state = get_volume_state(current);

        if (last_volume_state == current_volume_state) {
            // we did not hit a surface
            return false;
        }

        float iso_surface_density = surface_density_as_fraction_of_rest_density * rest_density;

        if (last_volume_state == VolumeEvaluationResult::VolumeState::Outside) {
            // we were outside the fluid
            float interpolation_factor = (iso_surface_density - last.density) / (current.density - last.density);
            result.point_of_intersection = interpolate_between_vectors(last.position, current.position, interpolation_factor);

            if (current_volume_state == VolumeEvaluationResult::VolumeState::Fluid) {
                // and reached the fluid
                result.intersection_result_type = IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid;
                result.normal_at_intersection = -1.0f * Math::safe_normalize(interpolate_between_vectors(last.fluid_only_normal, current.fluid_only_normal, interpolation_factor));
            } else if (current_volume_state == VolumeEvaluationResult::VolumeState::Boundary) {
                // and reached the boundary
                result.intersection_result_type = IntersectionResult::IntersectionResultType::RayHitBoundarySurface;
                result.normal_at_intersection = -1.0f * Math::safe_normalize(interpolate_between_vectors(last.boundary_only_normal, current.boundary_only_normal, interpolation_factor));
            }

        } else if (last_volume_state == VolumeEvaluationResult::VolumeState::Fluid) {
            // we were inside the fluid
            if (current_volume_state == VolumeEvaluationResult::VolumeState::Outside) {
                // and reached the outside
                result.intersection_result_type = IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid;

                float interpolation_factor = (last.density - iso_surface_density) / (last.density - current.density);
                result.point_of_intersection = interpolate_between_vectors(last.position, current.position, interpolation_factor);
                result.normal_at_intersection = -1.0f * Math::safe_normalize(interpolate_between_vectors(last.fluid_only_normal, current.fluid_only_normal, interpolation_factor));

            } else if (current_volume_state == VolumeEvaluationResult::VolumeState::Boundary) {
                // and reached a boundary

                result.intersection_result_type = IntersectionResult::IntersectionResultType::RayHitBoundarySurface;

                float interpolation_factor = (last.boundary_only_density - last.fluid_only_density) / (current.fluid_only_density - last.fluid_only_density - current.boundary_only_density + last.boundary_only_density);
                result.point_of_intersection = interpolate_between_vectors(last.position, current.position, interpolation_factor);
                result.normal_at_intersection = -1.0f * Math::safe_normalize(interpolate_between_vectors(last.boundary_only_normal, current.boundary_only_normal, interpolation_factor));
            }
        } else if (last_volume_state == VolumeEvaluationResult::VolumeState::Boundary) {
            // we were inside the boundary
            return false;
        }

        return true;
    }

    ParticleIntersectionAccelerator::VolumeEvaluationResult::VolumeState ParticleIntersectionAccelerator::get_volume_state(const ParticleIntersectionAccelerator::VolumeEvaluationResult& result) const {
        float iso_surface_density = surface_density_as_fraction_of_rest_density * rest_density;

        if (result.density < iso_surface_density) {
            return VolumeEvaluationResult::VolumeState::Outside;
        }

        if (result.fluid_only_density > result.boundary_only_density) {
            return VolumeEvaluationResult::VolumeState::Fluid;
        }

        return ParticleIntersectionAccelerator::VolumeEvaluationResult::VolumeState::Boundary;
    }

    glm::vec3 ParticleIntersectionAccelerator::interpolate_between_vectors(const glm::vec3& a, const glm::vec3& b, float factor) {
        return (1.0f - factor) * a + b * factor;
    }

    bool ParticleIntersectionAccelerator::is_intersecting_with_particles(Ray& ray) {
        IntersectionResult dump;
        return is_intersecting_with_particles(ray, dump);
    }


} // namespace LibFluid::Raytracer