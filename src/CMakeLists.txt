# Add engine
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/engine)

find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)
find_path(LIBMORTON_INCLUDE_DIRS "libmorton/morton.h")
find_package(cxxopts CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(unofficial-nativefiledialog CONFIG REQUIRED)

if (UNIX AND NOT APPLE)
        # add tbb library for parallelization
        # TODO: check if the library also works well on mac os, since clang does not support the TS Paralellization

        find_package(TBB CONFIG REQUIRED)
endif()



# include directories
include_directories(./)
include_directories(engine)


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
        core/fluidSolver/kernel/CubicSplineKernel.cpp
        core/fluidSolver/kernel/CubicSplineKernel.hpp
        core/interface/Constants.hpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.cpp
       # core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp
        core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp
        core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp
        FluidSolverConsole.cpp
        FluidSolverConsole.hpp
        core/interface/ISimulationModifier.hpp
       # core/simulationModifiers/DeathBox.cpp
       # core/simulationModifiers/DeathBox.hpp
       # core/simulationModifiers/SpawnArea.cpp
       # core/simulationModifiers/SpawnArea.hpp
        core/Simulation.cpp core/Simulation.hpp
       # core/interface/IExternalForce.hpp
        core/visualizer/ISimulationVisualizer.hpp
        core/fluidSolver/IFluidSolver.hpp
        core/fluidSolver/IISPHFluidSolver.cpp
        core/fluidSolver/IISPHFluidSolver.hpp
        core/timestep/ITimestep.cpp core/timestep/ITimestep.hpp
        core/timestep/ConstantTimestep.cpp
        core/timestep/ConstantTimestep.hpp
        core/timestep/DynamicCFLTimestep.cpp
        core/timestep/DynamicCFLTimestep.hpp
        core/visualizer/ContinousVisualizer.cpp
        core/visualizer/ContinousVisualizer.hpp
       # uiVersion/ContinousVisualizerOpenGL.cpp
       # uiVersion/ContinousVisualizerOpenGL.hpp
       # uiVersion/userInterface/MainUi.cpp
       # uiVersion/userInterface/MainUi.hpp
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
        core/sensors/SensorData.cpp
        core/sensors/SensorData.hpp
        core/sensors/ParticleStatistics.cpp
        core/sensors/ParticleStatistics.hpp
        core/FluidTypes.hpp
        core/FluidInclude.hpp 
        uiVersion/ImguiHelper.cpp uiVersion/ImguiHelper.hpp       
        uiVersion/userInterface/ScenariosWindow.cpp uiVersion/userInterface/ScenariosWindow.hpp
        core/group/GroupActions.cpp core/group/GroupActions.hpp core/group/RectangleAreaGroup.cpp
        core/group/RectangleAreaGroup.hpp
        core/group/TagGroup.cpp core/group/TagGroup.hpp
        core/entities/ParticleSpawner.cpp core/entities/ParticleSpawner.hpp
        core/entities/IEntity.hpp
        uiVersion/userInterface/UiLayer.cpp uiVersion/userInterface/UiLayer.hpp
        uiVersion/FluidSolverTypes.cpp uiVersion/FluidSolverTypes.hpp
        core/entities/ParticleRemover.cpp core/entities/ParticleRemover.hpp
        uiVersion/userInterface/StatisticsUi.cpp uiVersion/userInterface/StatisticsUi.hpp
        core/sensors/ISensor.hpp
        "Paths.hpp"
        "core/Log.hpp" "core/Log.cpp"
        "uiVersion/userInterface/LogWindow.cpp" "uiVersion/userInterface/LogWindow.hpp"
        "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp" "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.cpp"
        "core/parallelization/NoParallelization.hpp" "core/parallelization/NoParallelization.cpp"
        "core/parallelization/StdParallelForEach.hpp" "core/parallelization/StdParallelForEach.cpp"
        "core/serialization/SimulationSerializer.hpp" "core/serialization/SimulationSerializer.cpp"
        "core/visualizer/GLParticleRenderer.hpp" "core/visualizer/GLParticleRenderer.cpp"
        "core/visualizer/GLParticleVertexArray.hpp" "core/visualizer/GLParticleVertexArray.cpp"
        "core/visualizer/Image.hpp" "core/visualizer/Image.cpp"
        "core/sensors/SensorWriter.hpp" "core/sensors/SensorWriter.cpp"
        "core/OutputManager.hpp" "core/OutputManager.cpp"
        )


add_library(FluidSolver ${FLUID_SOLVER_SOURCE_FILES})
target_link_libraries(FluidSolver PUBLIC ${FLUIDSOLVER_SYSTEM_LIBS} ENGINE)
target_link_libraries(FluidSolver PUBLIC imgui::imgui)
target_link_libraries(FluidSolver PUBLIC implot::implot)
target_include_directories(FluidSolver PUBLIC ${LIBMORTON_INCLUDE_DIRS})
target_link_libraries(FluidSolver PUBLIC cxxopts::cxxopts)
target_link_libraries(FluidSolver PUBLIC fmt::fmt)
target_link_libraries(FluidSolver PUBLIC yaml-cpp)
target_link_libraries(FluidSolver PUBLIC unofficial::nativefiledialog::nfd)

if (UNIX AND NOT APPLE)
        # add tbb library for parallelization to the link targets
        target_link_libraries(FluidSolver PUBLIC TBB::tbb)
endif()


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLUID_SOLVER_SOURCE_FILES})

