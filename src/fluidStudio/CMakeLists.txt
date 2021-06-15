# Add engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)


# include directories
include_directories(./)
include_directories(../libFluid)
include_directories(./engine)


set(FLUID_STUDIO_SOURCE_FILES
        "main.cpp"
        "FluidSolverWindow.cpp"
        "FluidSolverWindow.hpp"
        "ImguiHelper.cpp" "ImguiHelper.hpp"
        "userInterface/ScenariosWindow.cpp" "userInterface/ScenariosWindow.hpp"
        "userInterface/UiLayer.cpp" "userInterface/UiLayer.hpp"
        "FluidSolverTypes.cpp" "FluidSolverTypes.hpp"
        "userInterface/StatisticsUi.cpp" "userInterface/StatisticsUi.hpp"       
        "Paths.hpp"     
        "userInterface/LogWindow.cpp" "userInterface/LogWindow.hpp"
        "visualizer/GLParticleRenderer.hpp" "visualizer/GLParticleRenderer.cpp"
        "visualizer/GLParticleVertexArray.hpp" "visualizer/GLParticleVertexArray.cpp"
        "visualizer/GLParticleRenderer3D.hpp" "visualizer/GLParticleRenderer3D.cpp"
        "visualizer/GLRenderer.hpp" "visualizer/GLRenderer.cpp"
        "visualizer/GLParticleVertexArray3D.hpp" "visualizer/GLParticleVertexArray3D.cpp"
        "incbin/incbin.h"
        "incbin/incbin_helper.hpp" "incbin/incbin_helper.cpp"        
        )


add_executable(FluidStudio ${FLUID_STUDIO_SOURCE_FILES})
target_link_libraries(FluidStudio PUBLIC libFluid)
target_link_libraries(FluidStudio PUBLIC ENGINE)
target_link_libraries(FluidStudio PUBLIC imgui::imgui)
target_link_libraries(FluidStudio PUBLIC implot::implot)
target_link_libraries(FluidStudio PUBLIC unofficial::nativefiledialog::nfd)


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLUID_STUDIO_SOURCE_FILES})

