#include "GLRenderer.hpp"
#include "engine/Window.hpp"

bool FluidSolver::GLRenderer::is_opengl_available()
{
    return Engine::opengl_context_available();
}
