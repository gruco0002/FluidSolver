#pragma once

#include "visualizer/raytracer/Ray.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"
#include <functional>
#include <glm/glm.hpp>

namespace LibFluid::Raytracer {
    class Camera {
      public:
        struct CameraSettings {
            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 view_direction = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 view_up = glm::vec3(0.0f, 1.0f, 0.0f);

            float field_of_view_x = 60.0f / 360.0f * 2.0f * 3.1415f;

            bool flip_y = false;

            RenderTarget* render_target = nullptr;
        } settings;

        struct SampleSettings {
            size_t amount_of_samples = 50;

        } sample_settings;

        void generate_image(const std::function<LightValue(Ray&)>& evaluate_ray);

      private:
        glm::mat4 view_matrix;
        glm::mat4 inverse_view_matrix;


        Ray generate_ray_for_sample_position(float x, float y);

        void update_view_matrix();
    };
} // namespace LibFluid::Raytracer