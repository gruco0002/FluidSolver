#pragma once

#include "engine/graphics/Texture2D.hpp"

#include <glm/glm.hpp>

// FIXME: wrong namespace
namespace LibFluid {

    class GLRenderer {
      public:
        virtual Engine::Graphics::Texture2D* get_render_target() = 0;

        static bool is_opengl_available();

        virtual void get_view(glm::vec3& position, glm::vec3& view_direction, glm::vec3& view_up) const = 0;
    };

} // namespace LibFluid