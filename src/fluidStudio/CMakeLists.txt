# find packages
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)
find_package(tinyply CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

# Define engine libs for OpenGL and window creation for the different platform
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


# include directories
include_directories(./)
include_directories(../libFluid)


set(FLUID_STUDIO_SOURCE_FILES
        "main.cpp"
        "FluidSolverWindow.cpp"
        "FluidSolverWindow.hpp"
        "ImguiHelper.cpp" "ImguiHelper.hpp"
        "userInterface/UiLayer.cpp" "userInterface/UiLayer.hpp"
        "FluidSolverTypes.cpp" "FluidSolverTypes.hpp"
        "userInterface/elements/SensorGraphWindows.cpp" "userInterface/elements/SensorGraphWindows.hpp"
        "userInterface/elements/LogWindow.cpp" "userInterface/elements/LogWindow.hpp"
        "visualizer/GLParticleRenderer.hpp" "visualizer/GLParticleRenderer.cpp"
        "visualizer/GLParticleVertexArray.hpp" "visualizer/GLParticleVertexArray.cpp"
        "visualizer/GLParticleRenderer3D.hpp" "visualizer/GLParticleRenderer3D.cpp"
        "visualizer/GLRenderer.hpp" "visualizer/GLRenderer.cpp"
        "visualizer/GLParticleVertexArray3D.hpp" "visualizer/GLParticleVertexArray3D.cpp"
        "incbin/incbin.h"
        "incbin/incbin_helper.hpp" "incbin/incbin_helper.cpp"
        "Assets.hpp" "Assets.cpp"
        "userInterface/elements/PlyImportWindow.hpp" "userInterface/elements/PlyImportWindow.cpp"
        "FluidStudioFwd.hpp"
        DirectoryHelper.cpp DirectoryHelper.hpp
        runners/ComponentRunner.cpp runners/ComponentRunner.hpp
        runners/SimulationRunner.cpp runners/SimulationRunner.hpp
        runners/VisualizationRunner.cpp runners/VisualizationRunner.hpp
        TimelineService.cpp TimelineService.hpp
        userInterface/elements/TimelineWindow.cpp userInterface/elements/TimelineWindow.hpp
        visualizationOverlay/VisualizationOverlay.cpp visualizationOverlay/VisualizationOverlay.hpp
        visualizationOverlay/OverlayCubeRenderer.cpp visualizationOverlay/OverlayCubeRenderer.hpp
        visualizationOverlay/OverlayInstance.hpp
        visualizationOverlay/ParticleRemoverOverlay.cpp visualizationOverlay/ParticleRemoverOverlay.hpp
        userInterface/elements/SimulationControlsWindow.cpp userInterface/elements/SimulationControlsWindow.hpp
        userInterface/UiData.hpp userInterface/UiData.cpp
        userInterface/elements/MainWindowMenu.cpp userInterface/elements/MainWindowMenu.hpp
        userInterface/UiElement.cpp userInterface/UiElement.hpp
        userInterface/UiElementCollection.hpp
        userInterface/elements/ComponentPanelWindow.cpp userInterface/elements/ComponentPanelWindow.hpp
        userInterface/helpers/SimulationComponent.cpp userInterface/helpers/SimulationComponent.hpp
        userInterface/elements/ComponentSettingsWindow.cpp userInterface/elements/ComponentSettingsWindow.hpp
        userInterface/StyledImGuiElements.cpp userInterface/StyledImGuiElements.hpp
        userInterface/elements/componentSettings/SensorSettingsElement.cpp userInterface/elements/componentSettings/SensorSettingsElement.hpp
        userInterface/elements/componentSettings/SolverSettingsElement.cpp userInterface/elements/componentSettings/SolverSettingsElement.hpp
        userInterface/elements/componentSettings/TimestepSettingsElement.cpp userInterface/elements/componentSettings/TimestepSettingsElement.hpp
        userInterface/elements/componentSettings/VisualizerSettingsElement.cpp userInterface/elements/componentSettings/VisualizerSettingsElement.hpp
        userInterface/elements/componentSettings/EntitySettingsElement.cpp userInterface/elements/componentSettings/EntitySettingsElement.hpp
        userInterface/helpers/TypeInformationProvider.cpp userInterface/helpers/TypeInformationProvider.hpp
        visualizationOverlay/ParticleSelectionByTagOverlay.cpp visualizationOverlay/ParticleSelectionByTagOverlay.hpp
        userInterface/elements/ObjImportWindow.cpp userInterface/elements/ObjImportWindow.hpp
        userInterface/elements/NewSimulationModalWindow.cpp userInterface/elements/NewSimulationModalWindow.hpp
        userInterface/helpers/ParticleCollectionHelper.cpp userInterface/helpers/ParticleCollectionHelper.hpp
        userInterface/elements/InsertParticlesWindow.cpp userInterface/elements/InsertParticlesWindow.hpp
        serializerExtensions/VseGlParticleRenderer3D.cpp serializerExtensions/VseGlParticleRenderer3D.hpp
        serializerExtensions/VseGlParticleRenderer.cpp serializerExtensions/VseGlParticleRenderer.hpp
        serializerExtensions/FluidStudioSerializerExtensions.cpp serializerExtensions/FluidStudioSerializerExtensions.hpp
        userInterface/elements/componentSettings/TagDescriptorSettingsElement.cpp userInterface/elements/componentSettings/TagDescriptorSettingsElement.hpp
        runners/EditorVisualizationRunner.cpp runners/EditorVisualizationRunner.hpp
        userInterface/elements/componentSettings/EditorVisualizerSettingsElement.cpp userInterface/elements/componentSettings/EditorVisualizerSettingsElement.hpp
        helpers/SimulatorHelpers.cpp helpers/SimulatorHelpers.hpp
        serializerExtensions/FluidStudioRootSerializerExt.cpp serializerExtensions/FluidStudioRootSerializerExt.hpp
        userInterface/helpers/FileDialogHelper.cpp userInterface/helpers/FileDialogHelper.hpp
        "engine/Window.cpp"
        "engine/Window.hpp"
        "engine/graphics/Texture2D.cpp"
        "engine/graphics/Texture2D.hpp"
        "engine/graphics/Texture2DArray.cpp"
        "engine/graphics/Texture2DArray.hpp"
        "engine/graphics/buffer/UniformBuffer.hpp"
        "engine/graphics/Framebuffer.cpp"
        "engine/graphics/Framebuffer.hpp"
        "engine/graphics/buffer/VertexArray.cpp"
        "engine/graphics/buffer/VertexArray.hpp"
        "engine/graphics/buffer/VertexBuffer.hpp"
        "engine/graphics/buffer/IndexBuffer.hpp"
        "engine/graphics/buffer/Buffer.cpp"
        "engine/graphics/buffer/Buffer.hpp"
        "engine/graphics/Shader.cpp"
        "engine/graphics/Shader.hpp"
        "engine/graphics/Enums.hpp"
        "engine/graphics/Enums.cpp"
        "engine/RectangleRenderer.cpp"
        "engine/RectangleRenderer.hpp"
        "engine/stb_implementation.cpp"
        )


add_executable(FluidStudio ${FLUID_STUDIO_SOURCE_FILES})
target_include_directories(FluidStudio PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(FluidStudio PUBLIC libFluid)
target_link_libraries(FluidStudio PUBLIC imgui::imgui)
target_link_libraries(FluidStudio PUBLIC implot::implot)
target_link_libraries(FluidStudio PUBLIC unofficial::nativefiledialog::nfd)
target_link_libraries(FluidStudio PUBLIC tinyply)
target_link_libraries(FluidStudio PUBLIC imguizmo::imguizmo)
target_link_libraries(FluidStudio PUBLIC ${ENGINE_SYSTEM_LIBS} Threads::Threads glm::glm glad::glad glfw)


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLUID_STUDIO_SOURCE_FILES})

