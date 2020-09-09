# Add engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

# add imgui
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libraries/imgui)


# Setting linker flags and library stuff for the different platforms
if (WIN32)
    find_package(OpenMP REQUIRED)
    set(FLUIDSOLVER_SYSTEM_LIBS OpenMP::OpenMP_CXX)
endif ()

if (APPLE)

    # this paths have to be adapted to work with the llvm clang compiler and libraries
    set(CMAKE_PREFIX_PATH /usr/local/opt/llvm)
    set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
    set(openmpDylibPath /usr/local/opt/llvm/lib/libomp.dylib)

    find_package(LLVM REQUIRED CONFIG)

    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    # Find the libraries that correspond to the LLVM components
    # that we wish to use
    llvm_map_components_to_libnames(llvm_libs support core irreader)

    include_directories(${LLVM_INCLUDE_DIRS})


    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

    set(FLUIDSOLVER_SYSTEM_LIBS ${llvm_libs} ${openmpDylibPath})
endif ()

if (UNIX AND NOT APPLE)
    # for Linux, BSD, Solaris, Minix

    find_package(OpenMP REQUIRED)

    set(FLUIDSOLVER_SYSTEM_LIBS pthread OpenMP::OpenMP_CXX)

endif ()


# include directories

include_directories(./)
include_directories(libraries)
include_directories(libraries/imgui)
include_directories(engine)
include_directories(engine/libraries)
include_directories(engine/libraries/Glad)
include_directories(engine/dependencies)


set(FLUID_SOLVER_SOURCE_FILES
        uiVersion/FluidSolverWindow.cpp
        uiVersion/FluidSolverWindow.hpp
        core/fluidSolver/IISPHFluidSolver.hpp
        core/fluidSolver/IISPHFluidSolver.cpp
        core/fluidSolver/SESPHFluidSolver.cpp
        core/fluidSolver/SESPHFluidSolver.hpp
        core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp
        core/FluidSolverException.cpp core/FluidSolverException.hpp
        core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp
        core/fluidSolver/kernel/IKernel.hpp
        core/fluidSolver/kernel/IKernel.cpp
        uiVersion/ParticleVertexArray.cpp
        uiVersion/ParticleVertexArray.hpp
        uiVersion/ParticleRenderer.cpp
        uiVersion/ParticleRenderer.hpp
        core/fluidSolver/kernel/CubicSplineKernel.cpp
        core/fluidSolver/kernel/CubicSplineKernel.hpp
        core/interface/Constants.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp
        core/basicScenarios/Scenario.hpp core/basicScenarios/Scenario.cpp
        core/basicScenarios/BoundaryTestScenario.cpp
        core/basicScenarios/BoundaryTestScenario.hpp
        core/basicScenarios/SimpleDamScenario.cpp
        core/basicScenarios/SimpleDamScenario.hpp
        core/basicScenarios/HugeDamScenario.cpp
        core/basicScenarios/HugeDamScenario.hpp
        core/basicScenarios/SimpleBoxScenario.cpp
        core/basicScenarios/SimpleBoxScenario.hpp
        core/statistics/DataLogger.cpp
        core/statistics/DataLogger.hpp
        FluidSolverConsole.cpp
        FluidSolverConsole.hpp
        core/basicScenarios/BoxWithHoleScenario.cpp
        core/basicScenarios/BoxWithHoleScenario.hpp
        core/interface/ISimulationModifier.hpp
        core/simulationModifiers/DeathBox.cpp
        core/simulationModifiers/DeathBox.hpp
        core/simulationModifiers/SpawnArea.cpp
        core/simulationModifiers/SpawnArea.hpp
        core/statistics/StatisticCollector.cpp
        core/statistics/StatisticCollector.hpp
        core/basicScenarios/MultipleHoleScenario.cpp
        core/basicScenarios/MultipleHoleScenario.hpp
        core/Simulation.cpp core/Simulation.hpp
        core/interface/IExternalForce.hpp
        core/visualizer/ISimulationVisualizer.hpp
        core/fluidSolver/IFluidSolver.hpp
        core/fluidSolver/IISPHFluidSolver.cpp
        core/fluidSolver/IISPHFluidSolver.hpp
        core/basicScenarios/SimpleBoxScenarioSmallerParticles.cpp
        core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp
        core/basicScenarios/SimpleBoxScenarioLargerParticles.cpp
        core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp
        core/timestep/ITimestep.cpp core/timestep/ITimestep.hpp
        core/timestep/ConstantTimestep.cpp
        core/timestep/ConstantTimestep.hpp
        core/timestep/DynamicCFLTimestep.cpp
        core/timestep/DynamicCFLTimestep.hpp
        core/visualizer/ContinousVisualizer.cpp
        core/visualizer/ContinousVisualizer.hpp
        uiVersion/ContinousVisualizerOpenGL.cpp
        uiVersion/ContinousVisualizerOpenGL.hpp
        uiVersion/userInterface/MainUi.cpp
        uiVersion/userInterface/MainUi.hpp
        uiVersion/IOpenGLVisualizer.hpp
        core/statistics/CachedStatisticCollector.cpp
        core/statistics/CachedStatisticCollector.hpp
        core/selection/ParticleSelection.cpp
        core/selection/ParticleSelection.hpp
        core/selection/IParticleSelection.hpp
        core/selection/NormalParticleSelection.cpp
        core/selection/NormalParticleSelection.hpp
        core/selection/AllParticleSelection.cpp
        core/selection/AllParticleSelection.hpp
        core/statistics/StatValue.cpp
        core/statistics/StatValue.hpp
        core/statistics/CachedStatValue.cpp
        core/statistics/CachedStatValue.hpp
        core/interface/TypeDefinitions.hpp
        core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.cpp
        core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp
        core/fluidSolver/particleCollection/ZIndexGridSorter.cpp
        core/fluidSolver/particleCollection/ZIndexGridSorter.hpp
        core/fluidSolver/kernel/PreCalculatedCubicSplineKernel.cpp
        core/fluidSolver/kernel/PreCalculatedCubicSplineKernel.hpp
        uiVersion/userInterface/NeighborhoodSearch.cpp
        uiVersion/userInterface/NeighborhoodSearch.hpp
        uiVersion/userInterface/Kernel.cpp
        uiVersion/userInterface/Kernel.hpp
        core/fluidSolver/ParticleCollection.cpp
        core/fluidSolver/ParticleCollection.hpp
        core/fluidSolver/Particle.cpp
        core/fluidSolver/Particle.hpp
        core/fluidSolver/FluidAssert.hpp
        core/fluidSolver/ParticleCollectionAlgorithm.cpp
        core/fluidSolver/ParticleCollectionAlgorithm.hpp

        )


add_library(FluidSolver ${FLUID_SOLVER_SOURCE_FILES})
target_link_libraries(FluidSolver PUBLIC ${FLUIDSOLVER_SYSTEM_LIBS} ENGINE IMGUI)
