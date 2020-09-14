# Add engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

# Setting linker flags and library stuff for the different platforms
if (WIN32)
    find_package(OpenMP REQUIRED)
    set(FLUIDSOLVER_SYSTEM_LIBS OpenMP::OpenMP_CXX)
    add_definitions(/bigobj)
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


find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_path(LIBMORTON_INCLUDE_DIRS "libmorton/morton.h")
find_package(cxxopts CONFIG REQUIRED)
find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")


  


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
        core/FluidSolverException.cpp
        core/FluidSolverException.hpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.cpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp
        uiVersion/ParticleVertexArray.cpp
        uiVersion/ParticleVertexArray.hpp
        uiVersion/ParticleRenderer.cpp
        uiVersion/ParticleRenderer.hpp
        core/fluidSolver/kernel/CubicSplineKernel.cpp
        core/fluidSolver/kernel/CubicSplineKernel.hpp
        core/interface/Constants.hpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.cpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp
       # core/basicScenarios/Scenario.hpp
       # core/basicScenarios/Scenario.cpp
       # core/basicScenarios/BoundaryTestScenario.cpp
       # core/basicScenarios/BoundaryTestScenario.hpp
       # core/basicScenarios/SimpleDamScenario.cpp
       # core/basicScenarios/SimpleDamScenario.hpp
       # core/basicScenarios/HugeDamScenario.cpp
       # core/basicScenarios/HugeDamScenario.hpp
       # core/basicScenarios/SimpleBoxScenario.cpp
       # core/basicScenarios/SimpleBoxScenario.hpp
       # FluidSolverConsole.cpp
       # FluidSolverConsole.hpp
       # core/basicScenarios/BoxWithHoleScenario.cpp
       # core/basicScenarios/BoxWithHoleScenario.hpp
        core/interface/ISimulationModifier.hpp
       # core/simulationModifiers/DeathBox.cpp
       # core/simulationModifiers/DeathBox.hpp
       # core/simulationModifiers/SpawnArea.cpp
       # core/simulationModifiers/SpawnArea.hpp
       # core/basicScenarios/MultipleHoleScenario.cpp
       # core/basicScenarios/MultipleHoleScenario.hpp
        core/Simulation.cpp core/Simulation.hpp
       # core/interface/IExternalForce.hpp
        core/visualizer/ISimulationVisualizer.hpp
        core/fluidSolver/IFluidSolver.hpp
        core/fluidSolver/IISPHFluidSolver.cpp
        core/fluidSolver/IISPHFluidSolver.hpp
       # core/basicScenarios/SimpleBoxScenarioSmallerParticles.cpp
       # core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp
       # core/basicScenarios/SimpleBoxScenarioLargerParticles.cpp
       # core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp
        core/timestep/ITimestep.cpp core/timestep/ITimestep.hpp
        core/timestep/ConstantTimestep.cpp
        core/timestep/ConstantTimestep.hpp
        core/timestep/DynamicCFLTimestep.cpp
        core/timestep/DynamicCFLTimestep.hpp
       # core/visualizer/ContinousVisualizer.cpp
       # core/visualizer/ContinousVisualizer.hpp
       # uiVersion/ContinousVisualizerOpenGL.cpp
       # uiVersion/ContinousVisualizerOpenGL.hpp
       # uiVersion/userInterface/MainUi.cpp
       # uiVersion/userInterface/MainUi.hpp
        uiVersion/IOpenGLVisualizer.hpp
       # core/selection/ParticleSelection.cpp
       # core/selection/ParticleSelection.hpp
       # core/selection/IParticleSelection.hpp
       # core/selection/NormalParticleSelection.cpp
       # core/selection/NormalParticleSelection.hpp
       # core/selection/AllParticleSelection.cpp
       # core/selection/AllParticleSelection.hpp
        core/interface/TypeDefinitions.hpp
       # core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.cpp
       # core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp
       # core/fluidSolver/particleCollection/ZIndexGridSorter.cpp
       # core/fluidSolver/particleCollection/ZIndexGridSorter.hpp
       # uiVersion/userInterface/NeighborhoodSearch.cpp
       # uiVersion/userInterface/NeighborhoodSearch.hpp
       # uiVersion/userInterface/Kernel.cpp
       # uiVersion/userInterface/Kernel.hpp
        core/fluidSolver/ParticleCollection.cpp
        core/fluidSolver/ParticleCollection.hpp
        core/fluidSolver/Particle.cpp
        core/fluidSolver/Particle.hpp
        core/FluidAssert.hpp
        core/fluidSolver/ParticleCollectionAlgorithm.cpp
        core/fluidSolver/ParticleCollectionAlgorithm.hpp
        core/statistics/Statistics.cpp
        core/statistics/Statistics.hpp
        core/statistics/ParticleStatistics.cpp
        core/statistics/ParticleStatistics.hpp
        core/FluidTypes.hpp core/FluidInclude.hpp uiVersion/ImguiHelper.cpp uiVersion/ImguiHelper.hpp core/scenario/Scenario.cpp core/scenario/Scenario.hpp core/scenario/ScenarioData.cpp core/scenario/ScenarioData.hpp core/scenario/ScriptInterface.cpp core/scenario/ScriptInterface.hpp uiVersion/userInterface/ScenariosWindow.cpp uiVersion/userInterface/ScenariosWindow.hpp)


add_library(FluidSolver ${FLUID_SOLVER_SOURCE_FILES})
target_link_libraries(FluidSolver PUBLIC ${FLUIDSOLVER_SYSTEM_LIBS} ENGINE)
target_link_libraries(FluidSolver PUBLIC imgui::imgui)
target_link_libraries(FluidSolver PUBLIC implot::implot)
target_include_directories(FluidSolver PUBLIC ${LIBMORTON_INCLUDE_DIRS})
target_link_libraries(FluidSolver PUBLIC cxxopts::cxxopts)
target_include_directories(FluidSolver PUBLIC ${CHAISCRIPT_INCLUDE_DIRS})


