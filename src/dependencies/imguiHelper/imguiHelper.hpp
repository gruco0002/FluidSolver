//
// Created by corbi on 03.01.2020.
//

#ifndef IMGUI_IMGUIHELPER_HPP
#define IMGUI_IMGUIHELPER_HPP

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>

#ifdef __linux__
// linux
// TODO: check if this is correct
#include <examples/imgui_impl_osx.h>

#endif
#ifdef _WIN32
// windows
#include <examples/imgui_impl_win32.h>
#endif
#ifdef __APPLE__
// os x
#include <examples/imgui_impl_osx.h>
#endif




#endif //IMGUI_IMGUIHELPER_HPP
