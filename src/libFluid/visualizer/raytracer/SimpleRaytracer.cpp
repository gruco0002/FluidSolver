#include "SimpleRaytracer.hpp"

#include "LibFluidMath.hpp"

#include <algorithm>

namespace LibFluid::Raytracer
{

    void SimpleRaytracer::initialize()
    {
        if (simulation_data.has_data_changed())
        {
            accelerator.rest_density = simulation_data.rest_density;
            accelerator.particle_size = simulation_data.particle_size;
            accelerator.particle_collection = simulation_data.collection;
            simulation_data.acknowledge_data_change();
        }

        if (parameters.has_data_changed())
        {
            parameters.acknowledge_data_change();

            render_target.set_size(parameters.render_target.width, parameters.render_target.height);
        }

        camera.settings.render_target = &render_target;
    }

    void SimpleRaytracer::create_compatibility_report(CompatibilityReport &report)
    {
        initialize();
    }

    void SimpleRaytracer::render()
    {
        if (!parameters.enabled)
        {
            return;
        }

        initialize();

        // prepare for rendering
        accelerator.prepare();
        render_target.clear();
        camera.prepare();
        camera.sample_settings.amount_of_samples = 1;
        settings.light_direction = glm::normalize(settings.light_direction);

        // render into render target
        camera.render_batch_of_samples_to_render_target([this](Ray &ray) { return this->evaluate_ray(ray); });

        // create an image out of the render target
        apply_tone_mapping_to_render_target();
    }

    void SimpleRaytracer::apply_tone_mapping_to_render_target()
    {
        buffered_image = std::make_unique<Image>(render_target.get_width(), render_target.get_height());
        tone_mapper.map_render_target_to_image(render_target, *buffered_image);
    }

    LibFluid::Image SimpleRaytracer::get_image_data()
    {
        if (buffered_image == nullptr)
        {
            return Image(0, 0);
        }

        return *buffered_image;
    }

    glm::vec3 SimpleRaytracer::normal_to_color(const glm::vec3 &normal)
    {
        return glm::vec3(normal / 2.0f + glm::vec3(0.5f));
    }

    glm::vec3 SimpleRaytracer::evaluate_ray(Ray &ray)
    {
        // Implementation of a basic raytracer

        // find the initial intersection of the ray, if there is any
        {
            IntersectionResult intersection_result{};
            if (accelerator.is_intersecting_with_particles(ray, intersection_result))
            {
                if (settings.output == Settings::Output::Normal)
                {
                    // return the surface normal as color
                    return normal_to_color(intersection_result.normal_at_intersection);
                }

                FLUID_ASSERT(settings.output == Settings::Output::Color);

                // check if the surface is in the shadow
                Ray shadow_ray;
                shadow_ray.starting_point = intersection_result.point_of_intersection;
                shadow_ray.solid_angle = 1.0f;
                shadow_ray.normalized_direction = -settings.light_direction;
                FLUID_ASSERT(shadow_ray);
                bool surface_is_in_shadow = accelerator.is_intersecting_with_particles(shadow_ray);

                float diffuse_factor = 0.0f;
                if (!surface_is_in_shadow)
                {
                    // TODO: check if normal_at_intersection is normalized
                    diffuse_factor = Math::max(
                        0.0f, glm::dot(-settings.light_direction, intersection_result.normal_at_intersection));
                }

                float total_light_factor = settings.ambient_strength + diffuse_factor;

                if (is_boundary(intersection_result))
                {
                    return settings.boundary_color * total_light_factor;
                }
                else
                {
                    return settings.fluid_color * total_light_factor;
                }
            }
            else
            {
                // no intersection, return the background color
                return settings.background_color;
            }
        }

        return glm::vec3(0.0f);
    }

    void SimpleRaytracer::set_view(const glm::vec3 &position, const glm::vec3 &view_direction, const glm::vec3 &view_up)
    {
        camera.settings.position = position;
        camera.settings.view_direction = view_direction;
        camera.settings.view_up = view_up;
    }

    bool SimpleRaytracer::is_boundary(const IntersectionResult &intersection_result)
    {
        switch (intersection_result.intersection_result_type)
        {
        case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromOutsideTheFluid:
        case IntersectionResult::IntersectionResultType::RayReachedFluidSurfaceFromInsideTheFluid: {
            return false;
        }
        case IntersectionResult::IntersectionResultType::RayHitBoundarySurface: {
            return true;
        }
        }

        FLUID_ASSERT(false);
        return false;
    }

} // namespace LibFluid::Raytracer
