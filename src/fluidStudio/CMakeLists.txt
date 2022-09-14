# Add engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)
find_package(tinyply CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)

# include directories
include_directories(./)
include_directories(../libFluid)
include_directories(./engine)


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
        "ExtendedSimulationSerializer.hpp" "ExtendedSimulationSerializer.cpp"
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
        userInterface/SimulationComponent.cpp userInterface/SimulationComponent.hpp
        userInterface/elements/ComponentSettingsWindow.cpp userInterface/elements/ComponentSettingsWindow.hpp
        userInterface/StyledImGuiElements.cpp userInterface/StyledImGuiElements.hpp
        userInterface/elements/componentSettings/SensorSettingsElement.cpp userInterface/elements/componentSettings/SensorSettingsElement.hpp
        )


add_executable(FluidStudio ${FLUID_STUDIO_SOURCE_FILES})
target_link_libraries(FluidStudio PUBLIC libFluid)
target_link_libraries(FluidStudio PUBLIC ENGINE)
target_link_libraries(FluidStudio PUBLIC imgui::imgui)
target_link_libraries(FluidStudio PUBLIC implot::implot)
target_link_libraries(FluidStudio PUBLIC unofficial::nativefiledialog::nfd)
target_link_libraries(FluidStudio PUBLIC tinyply)
target_link_libraries(FluidStudio PUBLIC imguizmo::imguizmo)


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLUID_STUDIO_SOURCE_FILES})

