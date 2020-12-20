cmake_minimum_required(VERSION 3.13)
project(ENGINE)

find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)

# Setting linker flags and library stuff for the different platforms
if (WIN32)
    set(ENGINE_SYSTEM_LIBS Opengl32 user32 Shlwapi)
endif ()

if (APPLE)

    include_directories(/System/Library/Frameworks)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)   
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

     # note that the order is important for setting the libs
    # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
    set(ENGINE_SYSTEM_LIBS X11 Xxf86vm GL dl)

endif ()

# packages
find_path(STB_INCLUDE_DIRS "stb.h")

find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# includes
include_directories(./)
  
# source files
set(ENGINE_SOURCE_FILES 
Window.cpp Window.hpp EventDelegate.hpp EngineException.cpp EngineException.hpp graphics/Texture2D.cpp graphics/Texture2D.hpp graphics/Texture2DArray.cpp graphics/Texture2DArray.hpp graphics/buffer/UniformBuffer.cpp graphics/buffer/UniformBuffer.hpp graphics/Framebuffer.cpp graphics/Framebuffer.hpp graphics/buffer/VertexArray.cpp graphics/buffer/VertexArray.hpp graphics/buffer/VertexBuffer.cpp graphics/buffer/VertexBuffer.hpp graphics/buffer/IndexBuffer.cpp graphics/buffer/IndexBuffer.hpp graphics/buffer/Buffer.cpp graphics/buffer/Buffer.hpp graphics/Shader.cpp graphics/Shader.hpp graphics/Enums.hpp graphics/Enums.cpp
text/Font.cpp text/Font.hpp text/TextRenderer.cpp text/TextRenderer.hpp RectangleRenderer.cpp RectangleRenderer.hpp stb_implementation.cpp)


# linking
add_library(ENGINE STATIC ${ENGINE_SOURCE_FILES})
target_include_directories(ENGINE PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(ENGINE PUBLIC ${ENGINE_SYSTEM_LIBS} Threads::Threads glm glad::glad glfw)

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCE_FILES})
