#pragma once
#include "RenderTarget.hpp"

#include <glm/glm.hpp>

namespace LibFluid::Raytracer {

    class Skybox {
      public:
        LightValue get_light_value_by_direction(const glm::vec3& normalized_direction) const;
    };


} // namespace FluidSolver::Raytracer