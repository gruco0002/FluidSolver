#pragma once

#include "engine/graphics/Texture2D.hpp"

namespace FluidSolver
{

    class GLRenderer {

      public:
        virtual Engine::Graphics::Texture2D* get_render_target() = 0;

        static bool is_opengl_available();
    };

} // namespace FluidSolver