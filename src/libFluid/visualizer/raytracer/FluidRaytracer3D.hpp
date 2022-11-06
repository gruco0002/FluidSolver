#pragma once


#include "visualizer/ISimulationVisualizer.hpp"

#include "visualizer/raytracer/Camera.hpp"
#include "visualizer/raytracer/ParticleIntersectionAccelerator.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"
#include "visualizer/raytracer/Sampler.hpp"
#include "visualizer/raytracer/Skybox.hpp"
#include "visualizer/raytracer/ToneMapper.hpp"

namespace LibFluid::Raytracer {
    class FluidRaytracer3D : public ISimulationVisualizer {
      public:
        Camera camera;
        ParticleIntersectionAccelerator accelerator;
        RenderTarget render_target;
        ToneMapper tone_mapper;
        Skybox skybox;
        Sampler sampler;

        struct Settings {
            size_t maximum_recursion_depth = 10;

            struct {
                glm::vec3 direction = glm::vec3(1.0f, -2.0f, 1.0f);
                glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
            } main_light;
        } settings;

      private:
        std::unique_ptr<Image> buffered_image = nullptr;

        LightValue evaluate_ray(Ray& ray);

      public:
        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
        void render() override;
        Image get_image_data() override;
    };
} // namespace LibFluid::Raytracer
