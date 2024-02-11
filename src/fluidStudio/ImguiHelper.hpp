#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

// These includes need to come after the imgui includes
#include <ImGuizmo.h>
#include <implot.h>

#ifdef __linux__
// linux
// TODO: check if this is correct

#endif
#ifdef _WIN32
// windows
// #include <imgui_impl_win32.h>
#endif
#ifdef __APPLE__
// os x

#endif

namespace ImGuiHelper
{

    void Init(GLFWwindow *window);
    void Uninit();

    void PreRender();
    void PostRender();

} // namespace ImGuiHelper
