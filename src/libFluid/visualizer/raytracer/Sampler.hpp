#pragma once

#include "visualizer/raytracer/Ray.hpp"
#include "visualizer/raytracer/IntersectionResult.hpp"

namespace LibFluid::Raytracer {
    class Sampler {
      public:

        Ray sample_cosine_weighted_hemisphere(const glm::vec3& normalized_up);


    };
} // namespace FluidSolver::Raytracer