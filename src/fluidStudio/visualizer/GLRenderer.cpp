#include "GLRenderer.hpp"

#include "engine/Window.hpp"

bool LibFluid::GLRenderer::is_opengl_available()
{
    return Engine::opengl_context_available();
}
