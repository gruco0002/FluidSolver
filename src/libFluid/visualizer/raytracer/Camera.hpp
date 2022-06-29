#pragma once

#include "visualizer/raytracer/Ray.hpp"
#include "visualizer/raytracer/RenderTarget.hpp"
#include <functional>
#include <glm/glm.hpp>

namespace FluidSolver::Raytracer {
    class Camera {
      public:
        struct CameraSettings {
            glm::vec3 position;
            glm::vec3 look_at;


        } settings;

        struct SampleSettings {
        } sample_settings;

        void generate_image(const std::function<LightValue(Ray&)>& evaluate_ray);
    };
} // namespace FluidSolver::Raytracer