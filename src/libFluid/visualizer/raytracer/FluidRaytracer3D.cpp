#include "FluidRaytracer3D.hpp"

namespace LibFluid::Raytracer {

    void FluidRaytracer3D::initialize() {
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();
        }

        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();

            render_target.set_size(parameters.render_target.width, parameters.render_target.height);

            // TODO: update cameras render target size
        }
    }

    void FluidRaytracer3D::create_compatibility_report(CompatibilityReport& report) {
        initialize();
    }

    void FluidRaytracer3D::render() {
        initialize();

        // prepare for rendering
        accelerator.prepare();
        render_target.clear();

        // render into render target
        camera.generate_image([this](Ray& ray) { return this->evaluate_ray(ray); });

        // create an image out of the render target
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

        LightValue result;

        struct {
            Ray ray;
            LightValue accumulated_weights;
        } current;

        // initialize current state
        current.ray = ray;

        // evaluate radiance along the path
        for (size_t current_depth = 0; current_depth < settings.maximum_recursion_depth; current_depth++) {
            // check for early termination of loop
            if (current.accumulated_weights.is_zero()) {
                // all further radiance contributions are weighted by zero
                break;
            }

            // evaluate one direct ray to a randomly sampled light source
            // TODO: implement

            // evaluate one ray and determine the next one
            IntersectionResult intersection_result;
            if (accelerator.is_intersecting_with_particles(ray, intersection_result)) {
                // the ray has hit something

                switch (intersection_result.intersection_result_type) {
                    case IntersectionResult::IntersectionResultType::FluidOutside: {

                        break;
                    }
                    case IntersectionResult::IntersectionResultType::FluidInside: {

                        break;
                    }
                    case IntersectionResult::IntersectionResultType::BoundaryOutside: {

                        break;
                    }
                    case IntersectionResult::IntersectionResultType::BoundaryInside: {

                        break;
                    }
                }


            } else {
                // no intersection, return the appropriate value from the skybox
                auto value = skybox.get_light_value_by_direction(current.ray.normalized_direction);

                // multiply by current weight and add to result
                value.mul(current.accumulated_weights);
                result.add(value);
            }
        }

        return result;
    }


} // namespace FluidSolver::Raytracer
