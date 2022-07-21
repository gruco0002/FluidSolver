find_package(Threads REQUIRED)
find_path(LIBMORTON_INCLUDE_DIRS "libmorton/morton.h")
find_package(fmt CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)

if (UNIX OR APPLE)
        # add tbb library for parallelization

        find_package(TBB CONFIG REQUIRED)
endif()



# include directories
include_directories(./)


set(LIB_FLUID_SOURCE_FILES       
        "fluidSolver/SESPHFluidSolver.hpp"
        "fluidSolver/kernel/CubicSplineKernel.cpp"
        "fluidSolver/kernel/CubicSplineKernel.hpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.cpp"
        "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.cpp"
        "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
        "Simulator.cpp" "Simulator.hpp"
        "visualizer/ISimulationVisualizer.hpp"
        "fluidSolver/IFluidSolver.hpp"        
        "fluidSolver/IISPHFluidSolver.hpp"
        "time/ITimestepGenerator.cpp"
        "time/ITimestepGenerator.hpp"
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
        "sensors/SensorData.cpp"
        "sensors/SensorData.hpp"
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
        "CompatibilityReport.hpp" "CompatibilityReport.cpp"
        "helper/stb_implementation.cpp"
        "serialization/YamlHelpers.hpp"
        "sensors/SensorPlane.hpp" "sensors/SensorPlane.cpp"
        "Forward.hpp"
        "FluidMath.hpp"
        "fluidSolver/IISPHFluidSolver3D.hpp"
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
        visualizer/raytracer/FluidRaytracer3D.cpp visualizer/raytracer/FluidRaytracer3D.hpp
        visualizer/raytracer/Camera.cpp visualizer/raytracer/Camera.hpp
        visualizer/raytracer/RenderTarget.cpp visualizer/raytracer/RenderTarget.hpp
        visualizer/raytracer/ParticleIntersectionAccelerator.cpp visualizer/raytracer/ParticleIntersectionAccelerator.hpp
        visualizer/raytracer/ToneMapper.cpp visualizer/raytracer/ToneMapper.hpp
        visualizer/raytracer/Ray.cpp visualizer/raytracer/Ray.hpp
        visualizer/raytracer/IntersectionResult.cpp visualizer/raytracer/IntersectionResult.hpp
        visualizer/raytracer/Skybox.cpp visualizer/raytracer/Skybox.hpp
        visualizer/raytracer/Sampler.cpp visualizer/raytracer/Sampler.hpp
        )


add_library(libFluid ${LIB_FLUID_SOURCE_FILES})
SET_TARGET_PROPERTIES(libFluid PROPERTIES PREFIX "")
target_include_directories(libFluid PUBLIC ${LIBMORTON_INCLUDE_DIRS})
target_include_directories(libFluid PUBLIC ${STB_INCLUDE_DIRS})
target_link_libraries(libFluid PUBLIC Threads::Threads)
target_link_libraries(libFluid PUBLIC fmt::fmt)
target_link_libraries(libFluid PUBLIC yaml-cpp)
target_link_libraries(libFluid PUBLIC lz4::lz4)

if (UNIX OR APPLE)
        # add tbb library for parallelization to the link targets
        target_link_libraries(libFluid PUBLIC TBB::tbb)
endif()


# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_FLUID_SOURCE_FILES})

