cmake_minimum_required(VERSION 3.7)
project(IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

# folder for glfw include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../GLFW/include)

set(IMGUI_SOURCE_FILES ../imgui/imgui.cpp ../imgui/imgui.h ../imgui/imgui_draw.cpp ../imgui/imconfig.h ../imgui/imgui_demo.cpp
        ../imgui/imgui_internal.h ../imgui/imgui_widgets.cpp ../imgui/imstb_rectpack.h ../imgui/imstb_textedit.h
        ../imgui/imstb_truetype.h ../imgui/imgui_impl_opengl3.cpp ../imgui/imgui_impl_opengl3.h
        ../imgui/imgui_impl_glfw.cpp ../imgui/imgui_impl_glfw.h imguiHelper.cpp imguiHelper.hpp)

# Setting linker flags and library stuff for the different platforms
if (WIN32)
    set(ADDITIONAL_SOURCES ../imgui/imgui_impl_win32.cpp ../imgui/imgui_impl_win32.h)
endif ()

if (APPLE)
    set(ADDITIONAL_SOURCES ../imgui/imgui_impl_osx.mm ../imgui/imgui_impl_osx.h)
endif ()

if (UNIX AND NOT APPLE)
    # TODO: check if this is correct
    set(ADDITIONAL_SOURCES ../imgui/imgui_impl_osx.mm ../imgui/imgui_impl_osx.h)
endif ()


add_library(IMGUI STATIC ${IMGUI_SOURCE_FILES} ${ADDITIONAL_SOURCES})