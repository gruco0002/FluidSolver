#include "FluidRaytracer3D.hpp"

#include "LibFluidMath.hpp"
#include "visualizer/raytracer/distributions/CosineWeightedHemisphereDistribution.hpp"

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

    LightValue FluidRaytracer3D::evaluate_ray(Ray& ray) {
        // Implementation of a basic stochastic path tracer


        struct {
            Ray ray;                         // the ray that hit the location
            IntersectionResult intersection; // the intersection of the ray
            LightValue accumulated_weights;
        } current;


        // find the initial intersection of the ray, if there is any
        {
            IntersectionResult intersection_result {};
            if (accelerator.is_intersecting_with_particles(ray, intersection_result)) {
                // the ray has hit something, initialize current state
                current.ray = ray;
                current.intersection = intersection_result;
                current.accumulated_weights = LightValue(1.0f);
            } else {
                // no intersection, return the appropriate value from the skybox
                auto value = skybox.get_light_value_by_direction(ray.normalized_direction);
                return value;
            }
        }

        LightValue result;
        // evaluate radiance along the path
        for (size_t current_depth = 0; current_depth < settings.maximum_recursion_depth; current_depth++) {
            // check for early termination of loop
            if (current.accumulated_weights.is_zero()) {
                // all further radiance contributions are weighted by zero
                break;
            }

            // importance sampling: sample the light source
            {
                Ray shadow_ray;
                {
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
                }

                if (!accelerator.is_intersecting_with_particles(shadow_ray)) {
                    // nothing was in the way
                    auto light_value = skybox.get_light_value_by_direction(shadow_ray.normalized_direction);

                    // weight the contribution by the solid angle (monte carlo weight)
                    light_value.mul(shadow_ray.solid_angle);

                    // cos between normal and shadow ray
                    light_value.mul(glm::dot(current.intersection.normal_at_intersection, shadow_ray.normalized_direction));

                    // incorporate brdf/bsdf
                    light_value.mul(bsdf(current.ray, current.intersection, shadow_ray));

                    // multiply with previous weights
                    light_value.mul(current.accumulated_weights);

                    // add to result
                    result.add(light_value);
                }
            }

            // sample a new ray and determine its intersection
            {
                auto new_ray = sample_at(current.intersection);

                IntersectionResult new_intersection_result {};
                if (accelerator.is_intersecting_with_particles(new_ray, new_intersection_result)) {
                    LightValue additional_weights(1.0f);

                    // weight the contribution by the solid angle (monte carlo weight)
                    additional_weights.mul(new_ray.solid_angle);

                    // cos between normal and shadow ray
                    additional_weights.mul(glm::dot(current.intersection.normal_at_intersection, new_ray.normalized_direction));

                    // incorporate brdf/bsdf
                    additional_weights.mul(bsdf(current.ray, current.intersection, new_ray));

                    // add those additional weights to the current weights
                    current.accumulated_weights.mul(additional_weights);

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

        return result;
    }

    Ray FluidRaytracer3D::sample_at(const IntersectionResult& intersection) {
        switch (intersection.intersection_result_type) {
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayHitBoundarySurface: {
                return Distributions::CosineWeightedHemisphereDistribution::sample_ray(sampler.get_uniform_sampled_pair(), intersection.normal_at_intersection);
            }
        }
    }

    LightValue FluidRaytracer3D::bsdf(const Ray& outgoing_radiance_ray, const IntersectionResult& intersection_result, const Ray& incoming_radiance_ray) {
        // we currently define the fluid and boundary as ideal lambertian surfaces (diffuse)
        const LightValue fluid_color(0.2f, 0.2f, 1.0f);
        const LightValue boundary_color(0.8f, 0.8f, 0.8f);

        switch (intersection_result.intersection_result_type) {
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid:
            case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid: {
                LightValue result(fluid_color);
                result.mul(1.0f / Math::PI);
                return result;
            }
            case IntersectionResult::IntersectionResultType::RayHitBoundarySurface: {
                LightValue result(boundary_color);
                result.mul(1.0f / Math::PI);
                return result;
            }
        }
    }

    void FluidRaytracer3D::set_view(const glm::vec3& position, const glm::vec3& view_direction, const glm::vec3& view_up) {
        camera.settings.position = position;
        camera.settings.view_direction = view_direction;
        camera.settings.view_up = view_up;
    }


} // namespace LibFluid::Raytracer
