#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
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
        float surface_density_as_fraction_of_rest_density = 0.8f;

        void prepare();

        bool is_intersecting_with_particles(Ray& ray);
        bool is_intersecting_with_particles(Ray& ray, IntersectionResult& result);

      private:
        AABB particle_surrounding_aabb;

        void calculate_aabb();

      private:
        CubicSplineKernel3D kernel;
        HashedNeighborhoodSearch3D neighborhood_search;

      private:
        struct VolumeEvaluationResult {
            enum class VolumeState {
                Outside,
                Fluid,
                Boundary
            };

            float density = 0.0f;
            float boundary_only_density = 0.0f;
            float fluid_only_density = 0.0f;
            glm::vec3 fluid_only_normal = glm::vec3(0.0f);
            glm::vec3 boundary_only_normal = glm::vec3(0.0f);

            glm::vec3 position = glm::vec3(0.0f);

        };

        VolumeEvaluationResult evaluate_volume_at_position(const glm::vec3& position);

        bool is_surface_intersected(const VolumeEvaluationResult& last, const VolumeEvaluationResult& current, IntersectionResult& result);


        VolumeEvaluationResult::VolumeState get_volume_state(const VolumeEvaluationResult& result) const;

        static glm::vec3 interpolate_between_vectors(const glm::vec3& a, const glm::vec3& b, float factor);

    };


} // namespace LibFluid::Raytracer