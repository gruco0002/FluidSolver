#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "visualizer/raytracer/AABB.hpp"
#include "visualizer/raytracer/IntersectionResult.hpp"
#include "visualizer/raytracer/Ray.hpp"

#include <memory>

namespace LibFluid::Raytracer {

    class ParticleIntersectionAccelerator {
      public:

        std::shared_ptr<ParticleCollection> particle_collection = nullptr;
        float particle_size = 0.1f;
        float rest_density = 1000.0f;

        void prepare();

        bool is_intersecting_with_particles(Ray& ray, IntersectionResult& result);

      private:
        AABB particle_surrounding_aabb;

        void calculate_aabb();

      private:
        struct VolumeEvaluationResult{
            float density;
            glm::vec3 normal;
        };

        VolumeEvaluationResult evaluate_volume_at_position(const glm::vec3& position);
    };


} // namespace LibFluid::Raytracer