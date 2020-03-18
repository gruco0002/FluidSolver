find_package(OpenGL REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)

# Add glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/Glad EXCLUDE_FROM_ALL)

# library glfw3.3
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLFW EXCLUDE_FROM_ALL)


# add imgui
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui)


# Setting linker flags and library stuff for the different platforms
if (WIN32)

    find_package(OpenMP REQUIRED)

    set(FLUIDSOLVER_LIBS glfw GLAD)
    set(FLUIDSOLVER_SYSTEM_LIBS OpenMP::OpenMP_CXX opengl32 user32 Shlwapi)
endif ()

if (APPLE)

    # this paths have to be adapted to work with the llvm clang compiler and libraries
    set(CMAKE_PREFIX_PATH /usr/local/opt/llvm)
    set(CMAKE_C_COMPILER   /usr/local/opt/llvm/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
    set(openmpDylibPath /usr/local/opt/llvm/lib/libomp.dylib)

    find_package(LLVM REQUIRED CONFIG)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    # Find the libraries that correspond to the LLVM components
    # that we wish to use
    llvm_map_components_to_libnames(llvm_libs support core irreader )

    include_directories( ${LLVM_INCLUDE_DIRS})


    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

    include_directories(/System/Library/Frameworks)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)

    set(FLUIDSOLVER_LIBS glfw GLAD)
    set(FLUIDSOLVER_SYSTEM_LIBS ${llvm_libs} ${openmpDylibPath} ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
endif ()

if (UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix

    find_package(OpenMP REQUIRED)

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
    set(FLUIDSOLVER_SYSTEM_LIBS X11 Xxf86vm GL dl pthread OpenMP::OpenMP_CXX)

endif ()




# include directories

include_directories(./)
include_directories(dependencies)
include_directories(dependencies/Glad)
include_directories(dependencies/imgui)
include_directories(dependencies/imguiHelper)


set(FLUID_SOLVER_SOURCE_FILES
        engine/Window.cpp engine/Window.hpp engine/EventDelegate.hpp engine/EngineException.cpp engine/EngineException.hpp uiVersion/FluidSolverWindow.cpp uiVersion/FluidSolverWindow.hpp engine/graphics/Texture2D.cpp engine/graphics/Texture2D.hpp engine/graphics/Texture2DArray.cpp engine/graphics/Texture2DArray.hpp engine/graphics/buffer/UniformBuffer.cpp engine/graphics/buffer/UniformBuffer.hpp libraries/stb/stb_loader.cpp engine/graphics/Framebuffer.cpp engine/graphics/Framebuffer.hpp engine/graphics/buffer/VertexArray.cpp engine/graphics/buffer/VertexArray.hpp engine/graphics/buffer/VertexBuffer.cpp engine/graphics/buffer/VertexBuffer.hpp engine/graphics/buffer/IndexBuffer.cpp engine/graphics/buffer/IndexBuffer.hpp engine/graphics/buffer/Buffer.cpp engine/graphics/buffer/Buffer.hpp engine/graphics/Shader.cpp engine/graphics/Shader.hpp engine/graphics/Enums.hpp engine/graphics/Enums.cpp core/fluidSolver/IISPHFluidSolver.hpp core/fluidSolver/IISPHFluidSolver.cpp
        engine/text/Font.cpp engine/text/Font.hpp engine/text/TextRenderer.cpp engine/text/TextRenderer.hpp engine/RectangleRenderer.cpp engine/RectangleRenderer.hpp core/fluidSolver/particleCollection/SimpleParticleCollection.cpp core/fluidSolver/particleCollection/SimpleParticleCollection.hpp core/fluidSolver/particleCollection/IParticleCollection.hpp core/fluidSolver/SPHFluidSolver.cpp core/fluidSolver/SPHFluidSolver.hpp core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp core/FluidSolverException.cpp core/FluidSolverException.hpp core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.cpp core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.cpp core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp core/fluidSolver/kernel/IKernel.hpp core/fluidSolver/kernel/IKernel.cpp uiVersion/ParticleVertexArray.cpp uiVersion/ParticleVertexArray.hpp uiVersion/ParticleRenderer.cpp uiVersion/ParticleRenderer.hpp core/fluidSolver/kernel/CubicSplineKernel.cpp core/fluidSolver/kernel/CubicSplineKernel.hpp core/interface/Constants.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.cpp core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp core/basicScenarios/Scenario.hpp core/basicScenarios/Scenario.cpp core/basicScenarios/BoundaryTestScenario.cpp core/basicScenarios/BoundaryTestScenario.hpp core/basicScenarios/SimpleDamScenario.cpp core/basicScenarios/SimpleDamScenario.hpp core/basicScenarios/HugeDamScenario.cpp core/basicScenarios/HugeDamScenario.hpp core/basicScenarios/SimpleBoxScenario.cpp core/basicScenarios/SimpleBoxScenario.hpp core/statistics/DataLogger.cpp core/statistics/DataLogger.hpp FluidSolverConsole.cpp FluidSolverConsole.hpp core/basicScenarios/BoxWithHoleScenario.cpp core/basicScenarios/BoxWithHoleScenario.hpp core/interface/ISimulationModifier.hpp core/simulationModifiers/DeathBox.cpp core/simulationModifiers/DeathBox.hpp core/simulationModifiers/SpawnArea.cpp core/simulationModifiers/SpawnArea.hpp core/statistics/StatisticCollector.cpp core/statistics/StatisticCollector.hpp core/basicScenarios/MultipleHoleScenario.cpp core/basicScenarios/MultipleHoleScenario.hpp core/Simulation.cpp core/Simulation.hpp core/interface/IExternalForce.hpp core/visualizer/ISimulationVisualizer.hpp core/fluidSolver/IFluidSolver.hpp core/fluidSolver/IISPHFluidSolver.cpp core/fluidSolver/IISPHFluidSolver.hpp core/basicScenarios/SimpleBoxScenarioSmallerParticles.cpp core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp core/basicScenarios/SimpleBoxScenarioLargerParticles.cpp core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp core/timestep/ITimestep.cpp core/timestep/ITimestep.hpp core/timestep/ConstantTimestep.cpp core/timestep/ConstantTimestep.hpp core/timestep/DynamicCFLTimestep.cpp core/timestep/DynamicCFLTimestep.hpp core/visualizer/ContinousVisualizer.cpp core/visualizer/ContinousVisualizer.hpp uiVersion/ContinousVisualizerOpenGL.cpp uiVersion/ContinousVisualizerOpenGL.hpp uiVersion/userInterface/MainUi.cpp uiVersion/userInterface/MainUi.hpp uiVersion/IOpenGLVisualizer.hpp core/statistics/CachedStatisticCollector.cpp core/statistics/CachedStatisticCollector.hpp core/selection/ParticleSelection.cpp core/selection/ParticleSelection.hpp core/selection/IParticleSelection.hpp core/selection/NormalParticleSelection.cpp core/selection/NormalParticleSelection.hpp core/selection/AllParticleSelection.cpp core/selection/AllParticleSelection.hpp core/statistics/StatValue.cpp core/statistics/StatValue.hpp core/statistics/CachedStatValue.cpp core/statistics/CachedStatValue.hpp core/fluidSolver/particleCollection/ParticleCollection.cpp core/fluidSolver/particleCollection/ParticleCollection.hpp core/fluidSolver/particleCollection/IParticleCollection.cpp)


add_library(FluidSolver ${FLUID_SOLVER_SOURCE_FILES})
target_link_libraries(FluidSolver PUBLIC ${FLUIDSOLVER_LIBS} ${FLUIDSOLVER_SYSTEM_LIBS} IMGUI)
