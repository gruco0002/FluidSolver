#ifndef IMGUI_IMGUIHELPER_HPP
#define IMGUI_IMGUIHELPER_HPP

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_plot.h>
#include <ImGuiFileDialog.h>

#ifdef __linux__
// linux
// TODO: check if this is correct


#endif
#ifdef _WIN32
// windows
#include <imgui_impl_win32.h>
#endif
#ifdef __APPLE__
// os x

#endif


namespace ImGuiHelper{

    void Init(GLFWwindow* window);
    void Uninit();

    void PreRender();
    void PostRender();

}



#endif //IMGUI_IMGUIHELPER_HPP
