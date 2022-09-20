find_package(Threads REQUIRED)
find_path(LIBMORTON_INCLUDE_DIRS "libmorton/morton.h")
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)

if (UNIX OR APPLE)
        # add tbb library for parallelization

        find_package(TBB CONFIG REQUIRED)
endif()



# include directories
include_directories(./)


set(LIB_FLUID_SOURCE_FILES       
        "fluidSolver/solver/SESPHFluidSolver.hpp"
        "fluidSolver/kernel/CubicSplineKernel.cpp"
        "fluidSolver/kernel/CubicSplineKernel.hpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
        "Simulator.cpp" "Simulator.hpp"
        "visualizer/ISimulationVisualizer.hpp"
        "fluidSolver/IFluidSolver.hpp"        
        "fluidSolver/solver/IISPHFluidSolver.hpp"
        "time/TimestepGenerator.cpp"
        "time/TimestepGenerator.hpp"
        "time/ConstantTimestepGenerator.cpp"
        "time/ConstantTimestepGenerator.hpp"
        "time/DynamicCflTimestepGenerator.cpp"
        "time/DynamicCflTimestepGenerator.hpp"
        "visualizer/ContinousVisualizer.cpp"
        "visualizer/ContinousVisualizer.hpp"
        "fluidSolver/ParticleCollection.hpp"
        "fluidSolver/Particle.hpp"
        "FluidAssert.hpp"
        "fluidSolver/ParticleCollectionAlgorithm.cpp"
        "fluidSolver/ParticleCollectionAlgorithm.hpp"
        "sensors/SensorDataStore.hpp"
        "sensors/ParticleStatistics.cpp"
        "sensors/ParticleStatistics.hpp"
        "FluidTypes.hpp"
        "FluidInclude.hpp"
        "group/GroupActions.hpp"
        "group/AreaGroup.cpp"
        "group/AreaGroup.hpp"
        "group/TagGroup.cpp"
        "group/TagGroup.hpp"
        "entities/ParticleSpawner.cpp"
        "entities/ParticleSpawner.hpp"
        "entities/SimulationEntity.hpp"
        "entities/ParticleRemover.cpp"
        "entities/ParticleRemover.hpp"
        "sensors/SensorBase.hpp"
        "Log.hpp" "Log.cpp"
        "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp" "fluidSolver/neighborhoodSearch/NeighborhoodInterface.cpp"
        "parallelization/NoParallelization.hpp" "parallelization/NoParallelization.cpp"
        "parallelization/StdParallelForEach.hpp" "parallelization/StdParallelForEach.cpp"
        "serialization/SimulationSerializer.hpp" "serialization/SimulationSerializer.cpp"      
        "visualizer/Image.hpp" "visualizer/Image.cpp"
        "sensors/OutputManager.hpp" "sensors/OutputManager.cpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp" "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.cpp"
        "fluidSolver/kernel/CubicSplineKernel3D.hpp" "fluidSolver/kernel/CubicSplineKernel3D.cpp"
        "fluidSolver/solver/SESPHFluidSolver3D.hpp"
        "visualizer/Viewport2D.hpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp" "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.cpp"
        "helper/ProtectedUnorderedMap.hpp"
        "CompatibilityReport.hpp" "CompatibilityReport.cpp"
        "helper/stb_implementation.cpp"
        "sensors/SensorPlane.hpp" "sensors/SensorPlane.cpp"
        "Forward.hpp"
        "FluidMath.hpp"
        "fluidSolver/solver/IISPHFluidSolver3D.hpp"
        "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp" "fluidSolver/neighborhoodSearch/CompressedNeighbors.cpp"
        "sensors/CompressedNeighborsStatistics.hpp" "sensors/CompressedNeighborsStatistics.cpp"
        "serialization/ParticleSerializer.cpp" "serialization/ParticleSerializer.hpp"
        "serialization/EndianSafeBinaryStream.hpp"
        "serialization/Lz4CompressedStream.cpp" "serialization/Lz4CompressedStream.hpp"
        "parallelization/SpinLock.cpp" "parallelization/SpinLock.hpp"
        "parallelization/AtomicFloat.cpp" "parallelization/AtomicFloat.hpp"
        time/Timepoint.hpp
        SimulatorVisualizerBundle.cpp SimulatorVisualizerBundle.hpp
        DataChangeStruct.cpp DataChangeStruct.hpp
        Initializable.hpp
        Reportable.hpp
        sensors/IisphSensor.cpp sensors/IisphSensor.hpp
        entities/ParticleRemover3D.cpp entities/ParticleRemover3D.hpp
        group/VolumeGroup.cpp group/VolumeGroup.hpp
        serialization/JsonHelpers.cpp serialization/JsonHelpers.hpp
        sensors/Sensor.hpp
        serialization/Base64.cpp serialization/Base64.hpp
        )


add_library(libFluid ${LIB_FLUID_SOURCE_FILES})
SET_TARGET_PROPERTIES(libFluid PROPERTIES PREFIX "")
target_include_directories(libFluid PUBLIC ${LIBMORTON_INCLUDE_DIRS})
target_include_directories(libFluid PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(libFluid PUBLIC Threads::Threads)
target_link_libraries(libFluid PUBLIC fmt::fmt)
target_link_libraries(libFluid PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(libFluid PUBLIC lz4::lz4)

if (UNIX OR APPLE)
        # add tbb library for parallelization to the link targets
        target_link_libraries(libFluid PUBLIC TBB::tbb)
endif()


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_FLUID_SOURCE_FILES})

