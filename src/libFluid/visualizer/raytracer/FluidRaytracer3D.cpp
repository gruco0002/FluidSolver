#include "FluidRaytracer3D.hpp"

namespace FluidSolver::Raytracer {

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

    FluidSolver::Image FluidRaytracer3D::get_image_data() {
        if (buffered_image == nullptr) {
            return Image(0, 0);
        }

        return *buffered_image;
    }

    LightValue FluidRaytracer3D::evaluate_ray(Ray& ray) {
        return LightValue();
    }

} // namespace FluidSolver::Raytracer
