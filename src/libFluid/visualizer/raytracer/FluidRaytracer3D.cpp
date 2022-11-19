#include "FluidRaytracer3D.hpp"

#include "LibFluidMath.hpp"
#include "visualizer/raytracer/distributions/CosineWeightedHemisphereDistribution.hpp"

#include <algorithm>

namespace LibFluid::Raytracer {

    void FluidRaytracer3D::initialize() {
        if (simulation_data.has_data_changed()) {
            accelerator.rest_density = simulation_data.rest_density;
            accelerator.particle_size = simulation_data.particle_size;
            accelerator.particle_collection = simulation_data.collection;
            simulation_data.acknowledge_data_change();
        }

        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();

            render_target.set_size(parameters.render_target.width, parameters.render_target.height);
        }

        camera.settings.render_target = &render_target;
        accelerator.sampler = &sampler;
    }

    void FluidRaytracer3D::create_compatibility_report(CompatibilityReport& report) {
        initialize();
    }

    void FluidRaytracer3D::render() {
        if (!parameters.enabled) {
            return;
        }

        initialize();

        // prepare for rendering
        accelerator.prepare();
        render_target.clear();
        camera.prepare();
        skybox.prepare();

        // render into render target
        render_additional_batch_of_samples();

        // create an image out of the render target
        apply_tone_mapping_to_render_target();
    }

    void FluidRaytracer3D::render_additional_batch_of_samples() {
        camera.render_batch_of_samples_to_render_target([this](Ray& ray) { return this->evaluate_ray(ray); });
    }

    void FluidRaytracer3D::apply_tone_mapping_to_render_target() {
        buffered_image = std::make_unique<Image>(render_target.get_width(), render_target.get_height());
        tone_mapper.map_render_target_to_image(render_target, *buffered_image);
    }

    LibFluid::Image FluidRaytracer3D::get_image_data() {
        if (buffered_image == nullptr) {
            return Image(0, 0);
        }

        return *buffered_image;
    }

    glm::vec3 normal_to_color(const glm::vec3& normal) {
        return glm::vec3(normal / 2.0f + glm::vec3(0.5f));
    }


    glm::vec3 FluidRaytracer3D::evaluate_ray(Ray& ray) {
        // Implementation of a basic stochastic path tracer


        struct {
            Ray ray;                         // the ray that hit the location
            IntersectionResult intersection; // the intersection of the ray
            glm::vec3 accumulated_weights;
        } current;


        // find the initial intersection of the ray, if there is any
        {
            IntersectionResult intersection_result {};
            if (accelerator.is_intersecting_with_particles(ray, intersection_result)) {
                // the ray has hit something, initialize current state
                current.ray = ray;
                current.intersection = intersection_result;
                current.accumulated_weights = glm::vec3(1.0f);

                if (settings.output_normals_of_first_hit) {
                    return normal_to_color(intersection_result.normal_at_intersection);
                }
            } else {
                // no intersection, return the appropriate value from the skybox
                if (!settings.output_normals_of_first_hit) {
                    return skybox.get_radiance_by_direction(ray.normalized_direction);
                } else {
                    // the normal is the opposite direction of the ray
                    return normal_to_color(-ray.normalized_direction);
                }
            }
        }

        glm::vec3 result_radiance(0.0f);

        // evaluate radiance along the path
        for (size_t current_depth = 0; current_depth < settings.maximum_recursion_depth; current_depth++) {
            // check for early termination of loop
            if (Math::is_zero(current.accumulated_weights)) {
                // all further radiance contributions are weighted by zero
                break;
            }

            // importance sampling: sample the light source
            {
                // start by sampling a ray towards the light source
                Ray shadow_ray;
                {
                    if (skybox.has_data()) {
                        // sample using the skybox sample distribution
                        shadow_ray.starting_point = current.intersection.point_of_intersection;
                        shadow_ray.length_until_hit = 0.0f;
                        float shadow_ray_pdf;
                        shadow_ray.normalized_direction = skybox.sample_normalized_direction(sampler.get_uniform_sampled_pair(), shadow_ray_pdf);
                        if (Math::is_not_zero(shadow_ray_pdf)) {
                            FLUID_ASSERT(Math::is_not_zero(shadow_ray_pdf));
                            shadow_ray.solid_angle = 1.0f / shadow_ray_pdf;
                        } else {
                            shadow_ray.solid_angle = 0.0f;
                        }
                    } else {
                        // the skybox is empty, fall back to cosine weighted hemisphere sampling on the surface
                        shadow_ray = Distributions::CosineWeightedHemisphereDistribution::sample_ray(sampler.get_uniform_sampled_pair(), current.intersection.normal_at_intersection);
                        shadow_ray.starting_point = current.intersection.point_of_intersection;
                        shadow_ray.length_until_hit = 0.0f;
                    }
                }

                // check if the light source is occluded by something
                if (!accelerator.is_intersecting_with_particles(shadow_ray)) {
                    // nothing was in the way
                    auto light_radiance = skybox.get_radiance_by_direction(shadow_ray.normalized_direction);

                    // weight the contribution by the solid angle (monte carlo weight)
                    light_radiance *= shadow_ray.solid_angle;

                    // cos between normal and shadow ray
                    float cosine_term = glm::dot(current.intersection.normal_at_intersection, shadow_ray.normalized_direction);
                    light_radiance *= std::clamp(cosine_term, 0.0f, 1.0f);

                    // incorporate brdf/bsdf
                    light_radiance *= bsdf(current.ray, current.intersection, shadow_ray);

                    // multiply with previous weights
                    light_radiance *= current.accumulated_weights;

                    // add to result
                    result_radiance += light_radiance;
                }
            }

            // sample a new ray and determine its intersection
            {
                auto new_ray = sample_at(current.intersection);

                IntersectionResult new_intersection_result {};
                if (accelerator.is_intersecting_with_particles(new_ray, new_intersection_result)) {
                    glm::vec3 additional_weights(1.0f);

                    // weight the contribution by the solid angle (monte carlo weight)
                    additional_weights *= new_ray.solid_angle;

                    // cos between normal and new ray
                    float cosine_term = glm::dot(current.intersection.normal_at_intersection, new_ray.normalized_direction);
                    additional_weights *= std::clamp(cosine_term, 0.0f, 1.0f);

                    // incorporate brdf/bsdf
                    additional_weights *= bsdf(current.ray, current.intersection, new_ray);

                    // add those additional weights to the current weights
                    current.accumulated_weights *= additional_weights;

                    // set the new ray as current ray
                    current.ray = new_ray;
                    current.intersection = new_intersection_result;

                    // then the loop continues and evaluates the new intersection

                } else {
                    // the ray did not hit anything, this means we are done at this place
                    // the ray would hit the skybox, but the skybox is already covered by the importance sampling above
                    break;
                }
            }
        }

        return result_radiance;
    }

    Ray FluidRaytracer3D::sample_at(const IntersectionResult& intersection) {
        switch (intersection.intersection_result_type) {
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayHitBoundarySurface: {
                auto ray = Distributions::CosineWeightedHemisphereDistribution::sample_ray(sampler.get_uniform_sampled_pair(), intersection.normal_at_intersection);
                ray.starting_point = intersection.point_of_intersection;
                ray.length_until_hit = 0.0f;
                return ray;
            }
        }
        FLUID_ASSERT(false);
        return {};
    }

    glm::vec3 FluidRaytracer3D::bsdf(const Ray& outgoing_radiance_ray, const IntersectionResult& intersection_result, const Ray& incoming_radiance_ray) {
        // we currently define the fluid and boundary as ideal lambertian surfaces (diffuse)
        glm::vec3 fluid_color = glm::vec3(0.0f, 0.6f, 1.0f);      // #0099ff
        glm::vec3 boundary_color = glm::vec3(1.0f, 0.733f, 0.0f); // #ffbb00


        switch (intersection_result.intersection_result_type) {
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid: {
                glm::vec3 result = fluid_color;
                result *= 1.0f / Math::PI;
                return result;
            }
            case IntersectionResult::IntersectionResultType::RayHitBoundarySurface: {
                glm::vec3 result = boundary_color;
                result *= 1.0f / Math::PI;
                return result;
            }
        }

        FLUID_ASSERT(false);
        return glm::vec3(0.0f);
    }

    void FluidRaytracer3D::set_view(const glm::vec3& position, const glm::vec3& view_direction, const glm::vec3& view_up) {
        camera.settings.position = position;
        camera.settings.view_direction = view_direction;
        camera.settings.view_up = view_up;
    }


} // namespace LibFluid::Raytracer
