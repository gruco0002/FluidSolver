cmake_minimum_required(VERSION 3.13)
project(ENGINE)

find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)


# Add glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libraries/Glad EXCLUDE_FROM_ALL)

# library glfw3.3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLFW EXCLUDE_FROM_ALL)

# Setting linker flags and library stuff for the different platforms
if (WIN32)


    set(ENGINE_LIBS glfw GLAD)
    set(ENGINE_SYSTEM_LIBS Opengl32 user32 Shlwapi)
endif ()

if (APPLE)

    include_directories(/System/Library/Frameworks)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)

    set(ENGINE_LIBS glfw GLAD)
    set(ENGINE_SYSTEM_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
endif ()

if (UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix

    add_definitions(${OPENGL_DEFINITIONS})
    find_package(X11 REQUIRED)

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH "./")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    set(ENGINE_LIBS glfw GLAD)

    # note that the order is important for setting the libs
    # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
    set(ENGINE_SYSTEM_LIBS X11 Xxf86vm GL dl pthread)

endif ()


# include directories

include_directories(./)
include_directories(libraries)
include_directories(libraries/Glad)
include_directories(dependencies)

# source files


set(ENGINE_SOURCE_FILES 
Window.cpp Window.hpp EventDelegate.hpp EngineException.cpp EngineException.hpp graphics/Texture2D.cpp graphics/Texture2D.hpp graphics/Texture2DArray.cpp graphics/Texture2DArray.hpp graphics/buffer/UniformBuffer.cpp graphics/buffer/UniformBuffer.hpp libraries/stb/stb_loader.cpp graphics/Framebuffer.cpp graphics/Framebuffer.hpp graphics/buffer/VertexArray.cpp graphics/buffer/VertexArray.hpp graphics/buffer/VertexBuffer.cpp graphics/buffer/VertexBuffer.hpp graphics/buffer/IndexBuffer.cpp graphics/buffer/IndexBuffer.hpp graphics/buffer/Buffer.cpp graphics/buffer/Buffer.hpp graphics/Shader.cpp graphics/Shader.hpp graphics/Enums.hpp graphics/Enums.cpp
text/Font.cpp text/Font.hpp text/TextRenderer.cpp text/TextRenderer.hpp RectangleRenderer.cpp RectangleRenderer.hpp )




add_library(ENGINE STATIC ${ENGINE_SOURCE_FILES})
target_link_libraries(ENGINE PUBLIC ${ENGINE_LIBS} ${ENGINE_SYSTEM_LIBS})
