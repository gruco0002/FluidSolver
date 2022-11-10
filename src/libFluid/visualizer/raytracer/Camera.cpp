#include "Camera.hpp"

#include "LibFluidAssert.hpp"

#include <glm/ext/matrix_transform.hpp>


namespace LibFluid::Raytracer {

    Ray Camera::generate_ray_for_sample_position(float x, float y) {
        float rel_x = (x + 0.5f) / (float)settings.render_target->get_width();
        float rel_y = (y + 0.5f) / (float)settings.render_target->get_height();

        rel_x *= 2.0f;
        rel_x -= 1.0f;
        rel_y *= 2.0f;
        rel_y -= 1.0f;

        if (settings.flip_y) {
            rel_y *= -1.0f;
        }

        float aspect_ratio = (float)settings.render_target->get_width() / (float)settings.render_target->get_width();

        float fov_y = settings.field_of_view_x / aspect_ratio;

        float x_view = rel_x * aspect_ratio * std::tan(settings.field_of_view_x / 2.0f);
        float y_view = rel_y * std::tan(settings.field_of_view_x / 2.0f);

        glm::vec3 pixel_pos = glm::normalize(glm::vec3(x_view, y_view, 1.0f));

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

    void Camera::generate_image(const std::function<LightValue(Ray&)>& evaluate_ray) {
        FLUID_ASSERT(settings.render_target != nullptr);

        update_view_matrix();

        // generate sample positions
        for (size_t pixel_x = 0; pixel_x < settings.render_target->get_width(); pixel_x++) {
            for (size_t pixel_y = 0; pixel_y < settings.render_target->get_height(); pixel_y++) {
                LightValue light_value;

                for (size_t counter = 0; counter < sample_settings.amount_of_samples; counter++) {
                    // TODO: sample around pixel_x and pixel_y and weight samples accordingly
                    auto ray = generate_ray_for_sample_position((float)pixel_x, (float)pixel_y);
                    auto result = evaluate_ray(ray);
                    light_value.add(result);
                }
                light_value.mul(1.0f / (float)sample_settings.amount_of_samples);


                settings.render_target->set(pixel_x, pixel_y, light_value);
            }
        }
    }
} // namespace LibFluid::Raytracer