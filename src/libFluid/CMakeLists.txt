find_package(Threads REQUIRED)
find_path(LIBMORTON_INCLUDE_DIRS "libmorton/morton.h")
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if (UNIX AND NOT APPLE)
        # add tbb library for parallelization
        # TODO: check if the library also works well on mac os, since clang does not support the TS Paralellization

        find_package(TBB CONFIG REQUIRED)
endif()



# include directories
include_directories(./)


set(LIB_FLUID_SOURCE_FILES
       
        "fluidSolver/IISPHFluidSolver.hpp"
        "fluidSolver/IISPHFluidSolver.cpp"
        "fluidSolver/SESPHFluidSolver.cpp"
        "fluidSolver/SESPHFluidSolver.hpp"
        "FluidSolverException.cpp"
        "FluidSolverException.hpp"
        "fluidSolver/kernel/CubicSplineKernel.cpp"
        "fluidSolver/kernel/CubicSplineKernel.hpp"
        "interface/Constants.hpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
        "interface/ISimulationModifier.hpp"
        "Simulation.cpp" "Simulation.hpp"
        "visualizer/ISimulationVisualizer.hpp"
        "fluidSolver/IFluidSolver.hpp"
        "fluidSolver/IISPHFluidSolver.cpp"
        "fluidSolver/IISPHFluidSolver.hpp"
        "timestep/ITimestep.cpp"
        "timestep/ITimestep.hpp"
        "timestep/ConstantTimestep.cpp"
        "timestep/ConstantTimestep.hpp"
        "timestep/DynamicCFLTimestep.cpp"
        "timestep/DynamicCFLTimestep.hpp"
        "visualizer/ContinousVisualizer.cpp"
        "visualizer/ContinousVisualizer.hpp"
        "interface/TypeDefinitions.hpp"
        "fluidSolver/ParticleCollection.cpp"
        "fluidSolver/ParticleCollection.hpp"
        "fluidSolver/Particle.cpp"
        "fluidSolver/Particle.hpp"
        "FluidAssert.hpp"
        "fluidSolver/ParticleCollectionAlgorithm.cpp"
        "fluidSolver/ParticleCollectionAlgorithm.hpp"
        "sensors/SensorData.cpp"
        "sensors/SensorData.hpp"
        "sensors/ParticleStatistics.cpp"
        "sensors/ParticleStatistics.hpp"
        "FluidTypes.hpp"
        "FluidInclude.hpp"
        "group/GroupActions.cpp"
        "group/GroupActions.hpp"
        "group/RectangleAreaGroup.cpp"
        "group/RectangleAreaGroup.hpp"
        "group/TagGroup.cpp"
        "group/TagGroup.hpp"
        "entities/ParticleSpawner.cpp"
        "entities/ParticleSpawner.hpp"
        "entities/IEntity.hpp"
        "entities/ParticleRemover.cpp"
        "entities/ParticleRemover.hpp"
        "sensors/ISensor.hpp"
        "Log.hpp" "Log.cpp"
        "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp" "fluidSolver/neighborhoodSearch/NeighborhoodInterface.cpp"
        "parallelization/NoParallelization.hpp" "parallelization/NoParallelization.cpp"
        "parallelization/StdParallelForEach.hpp" "parallelization/StdParallelForEach.cpp"
        "serialization/SimulationSerializer.hpp" "serialization/SimulationSerializer.cpp"      
        "visualizer/Image.hpp" "visualizer/Image.cpp"
        "sensors/SensorWriter.hpp" "sensors/SensorWriter.cpp"
        "OutputManager.hpp" "OutputManager.cpp"      
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp" "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.cpp"
        "fluidSolver/kernel/CubicSplineKernel3D.hpp" "fluidSolver/kernel/CubicSplineKernel3D.cpp"
        "fluidSolver/SESPHFluidSolver3D.hpp"
        "visualizer/Viewport2D.hpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp" "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.cpp"
        "helper/ProtectedUnorderedMap.hpp"
        "Compatibility.hpp" "Compatibility.cpp"
        "helper/stb_implementation.cpp"
        "serialization/YamlHelpers.hpp"
        "sensors/SensorPlane.hpp" "sensors/SensorPlane.cpp"
        "Forward.hpp"
        "FluidMath.hpp"
        )


add_library(libFluid ${LIB_FLUID_SOURCE_FILES})
SET_TARGET_PROPERTIES(libFluid PROPERTIES PREFIX "")
target_include_directories(libFluid PUBLIC ${LIBMORTON_INCLUDE_DIRS})
target_include_directories(libFluid PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(libFluid PUBLIC Threads::Threads)
target_link_libraries(libFluid PUBLIC fmt::fmt)
target_link_libraries(libFluid PUBLIC yaml-cpp)
target_link_libraries(libFluid PUBLIC nlohmann_json)

if (UNIX AND NOT APPLE)
        # add tbb library for parallelization to the link targets
        target_link_libraries(libFluid PUBLIC TBB::tbb)
endif()


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_FLUID_SOURCE_FILES})

