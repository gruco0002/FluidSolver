#pragma once

#include "visualizer/raytracer/Ray.hpp"
#include "visualizer/raytracer/IntersectionResult.hpp"

namespace LibFluid::Raytracer {
    class ParticleIntersectionAccelerator {

      public:

        void prepare();

        bool is_intersecting_with_particles(Ray& ray, IntersectionResult& result);
    };
}