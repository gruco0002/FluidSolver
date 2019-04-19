find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)

# Add glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Glad EXCLUDE_FROM_ALL)

# library glfw3.3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLFW EXCLUDE_FROM_ALL)


# Setting linker flags and library stuff for the different platforms
if (WIN32)

    set(FLUIDSOLVER_LIBS glfw GLAD)
    set(FLUIDSOLVER_SYSTEM_LIBS opengl32 user32 Shlwapi)
endif ()

if (APPLE)

    include_directories(/System/Library/Frameworks)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)

    set(FLUIDSOLVER_LIBS glfw GLAD)
    set(FLUIDSOLVER_SYSTEM_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
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

    set(FLUIDSOLVER_LIBS glfw GLAD)

    # note that the order is important for setting the libs
    # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
    set(FLUIDSOLVER_SYSTEM_LIBS X11 Xxf86vm GL dl pthread)

endif ()

# include directories

include_directories(./)
include_directories(dependencies)
include_directories(dependencies/Glad)


set(FLUID_SOLVER_SOURCE_FILES
        engine/Window.cpp engine/Window.hpp engine/EventDelegate.hpp engine/EngineException.cpp engine/EngineException.hpp FluidSolverWindow.cpp FluidSolverWindow.hpp engine/graphics/Texture2D.cpp engine/graphics/Texture2D.hpp libraries/stb/stb_loader.cpp engine/graphics/Framebuffer.cpp engine/graphics/Framebuffer.hpp engine/graphics/buffer/VertexArray.cpp engine/graphics/buffer/VertexArray.hpp engine/graphics/buffer/VertexBuffer.cpp engine/graphics/buffer/VertexBuffer.hpp engine/graphics/buffer/IndexBuffer.cpp engine/graphics/buffer/IndexBuffer.hpp engine/graphics/buffer/Buffer.cpp engine/graphics/buffer/Buffer.hpp engine/graphics/Shader.cpp engine/graphics/Shader.hpp engine/graphics/Enums.hpp engine/graphics/Enums.cpp)


add_library(FluidSolver ${FLUID_SOLVER_SOURCE_FILES})
target_link_libraries(FluidSolver ${FLUIDSOLVER_LIBS} ${FLUIDSOLVER_SYSTEM_LIBS})