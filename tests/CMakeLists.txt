project(FluidSolver_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(glm CONFIG REQUIRED)
find_package(Threads REQUIRED)

set(FluidSolverTests_SOURCE_FILES
        main.cpp
        basic_check.cpp
        CubicSplineKernelTest.cpp
        # CompactHashingComponentTests/CompactHashingCellStorageTests.cpp 
        # CompactHashingComponentTests/CompactHashingHashTableTests.cpp
        NeighborhoodSearchTests.cpp  "CompressedNeighborhoodSearchComponentTests/NeighborhoodStorageTests.cpp" "ParticleCollectionTests/PCQuickSortTest.cpp" "ParticleCollectionTests/PCQuickSortStableTest.cpp")


#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#set(GOOGLETEST_VERSION 1.10.0)

# GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src/libFluid)

# Unit Tests
# Add test cpp file
add_executable( runUnitTests ${FluidSolverTests_SOURCE_FILES})
# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests libFluid GTest::gtest  GTest::gmock)
add_test( runUnitTests  runUnitTests )

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FluidSolverTests_SOURCE_FILES})

