#pragma once

#include "visualizer/ISimulationVisualizer.hpp"

#include "visualizer/raytracer/Camera.hpp"
#include "visualizer/raytracer/ParticleIntersectionAccelerator.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"
#include "visualizer/raytracer/Sampler.hpp"
#include "visualizer/raytracer/Skybox.hpp"
#include "visualizer/raytracer/ToneMapper.hpp"

namespace LibFluid::Raytracer {

    class SimpleRaytracer : public ISimulationVisualizer {
      public:
        Camera camera;
        ParticleIntersectionAccelerator accelerator;
        RenderTarget render_target;
        ToneMapper tone_mapper;

        struct Settings {
            enum class Output {
                Color,
                Normal
            } output = Output::Color;

            glm::vec3 light_direction = glm::vec3(1.0f, -1.0f, 1.0f);
            float ambient_strength = 0.2f;

            glm::vec3 background_color = glm::vec3(1.0f);
            glm::vec3 boundary_color = glm::vec3(0.4f);
            glm::vec3 fluid_color = glm::vec3(0.1f, 0.1f, 1.0f);
        } settings;

        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
        void render() override;
        Image get_image_data() override;
        void set_view(const glm::vec3& position, const glm::vec3& view_direction, const glm::vec3& view_up) override;

        void apply_tone_mapping_to_render_target();

      private:
        std::unique_ptr<Image> buffered_image = nullptr;

        glm::vec3 evaluate_ray(Ray& ray);

        bool is_boundary(const IntersectionResult& intersection_result);

        static glm::vec3 normal_to_color(const glm::vec3& normal);
    };

} // namespace LibFluid::Raytracer