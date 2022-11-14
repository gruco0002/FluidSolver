#include "Camera.hpp"

#include "LibFluidAssert.hpp"
#include "parallelization/StdParallelForEach.hpp"

#include <glm/ext/matrix_transform.hpp>


namespace LibFluid::Raytracer {

    Ray Camera::generate_ray_for_sample_position(float x, float y) {
        float rel_x = (x + 0.5f) / (float)settings.render_target->get_width();
        float rel_y = (y + 0.5f) / (float)settings.render_target->get_height();

        rel_x *= 2.0f;
        rel_x -= 1.0f;
        rel_y *= 2.0f;
        rel_y -= 1.0f;

        if (!settings.flip_y) {
            rel_y *= -1.0f;
        }

        float aspect_ratio = (float)settings.render_target->get_width() / (float)settings.render_target->get_width();

        float fov_y = settings.field_of_view_x / aspect_ratio;

        float x_view = rel_x * aspect_ratio * std::tan(settings.field_of_view_x / 2.0f);
        float y_view = rel_y * std::tan(settings.field_of_view_x / 2.0f);

        glm::vec3 pixel_pos = glm::normalize(glm::vec3(x_view, y_view, -1.0f));

        // transform direction to world space
        glm::vec4 transformed = glm::vec4(pixel_pos, 0.0f);
        transformed = inverse_view_matrix * transformed;

        // create ray
        Ray ray;
        ray.normalized_direction = glm::normalize(transformed);
        ray.solid_angle = settings.field_of_view_x / (float)settings.render_target->get_width();
        ray.starting_point = settings.position;

        return ray;
    }

    void Camera::update_view_matrix() {
        view_matrix = glm::lookAt(settings.position, settings.position + settings.view_direction, settings.view_up);
        inverse_view_matrix = glm::inverse(view_matrix);
    }

    void Camera::render_batch_of_samples_to_render_target(const std::function<LightValue(Ray&)>& evaluate_ray) {
        FLUID_ASSERT(settings.render_target != nullptr);

        // generate sample positions
        size_t width = settings.render_target->get_width();
        size_t height = settings.render_target->get_height();

        using Parallel = StdParallelForEach;
        // generate rays for each pixel

        Parallel::loop_for(0, width * height, [&](size_t i) {
            // determine x and y position
            size_t pixel_x = i % width;
            size_t pixel_y = (i - pixel_x) / width;

            LightValue light_value;

            // check if we rendered any samples before
            if (sample_per_pixel_counter > 0) {
                // scale the old data back up again
                light_value = settings.render_target->get(pixel_x, pixel_y);
                light_value.mul(sample_per_pixel_counter);
            }

            // generate the rays for the current pixel
            for (size_t counter = 0; counter < sample_settings.amount_of_samples; counter++) {
                // TODO: sample around pixel_x and pixel_y and weight samples accordingly
                auto ray = generate_ray_for_sample_position((float)pixel_x, (float)pixel_y);
                auto result = evaluate_ray(ray);
                light_value.add(result);
            }

            // take the total amount of samples into account
            light_value.mul(1.0f / (float)(sample_settings.amount_of_samples + sample_per_pixel_counter));

            settings.render_target->set(pixel_x, pixel_y, light_value);
        });

        // update the total amount of samples rendered to each pixel
        sample_per_pixel_counter += sample_settings.amount_of_samples;
    }

    void Camera::prepare() {
        FLUID_ASSERT(settings.render_target != nullptr);

        sample_per_pixel_counter = 0;
        update_view_matrix();
    }
} // namespace LibFluid::Raytracer