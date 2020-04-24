cmake_minimum_required(VERSION 3.7)
project(IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# folder for glfw include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../GLFW/include)

set(IMGUI_SOURCE_FILES imgui.cpp imgui.h imgui_draw.cpp imconfig.h imgui_demo.cpp
        imgui_internal.h imgui_widgets.cpp imstb_rectpack.h imstb_textedit.h
        imstb_truetype.h imgui_impl_opengl3.cpp imgui_impl_opengl3.h
        imgui_impl_glfw.cpp imgui_impl_glfw.h imguiHelper.cpp imguiHelper.hpp imgui_plot.cpp imgui_plot.h ImGuiFileDialog.cpp ImGuiFileDialog.h)

# Setting linker flags and library stuff for the different platforms
if (WIN32)
    set(ADDITIONAL_SOURCES imgui_impl_win32.cpp imgui_impl_win32.h)
endif ()

if (APPLE)
    set(ADDITIONAL_SOURCES imgui_impl_osx.mm imgui_impl_osx.h)
endif ()

if (UNIX AND NOT APPLE)
    # TODO: check if this is correct
endif ()


add_library(IMGUI STATIC ${IMGUI_SOURCE_FILES} ${ADDITIONAL_SOURCES})