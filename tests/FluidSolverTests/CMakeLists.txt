include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR})

set(FluidSolverTests_SOURCE_FILES
        basic_check.cpp
        CubicSplineKernelTest.cpp NeighborhoodSearchTests.cpp CompactHashingComponentTests/CompactHashingCellStorageTests.cpp CompactHashingComponentTests/CompactHashingHashTableTests.cpp)

add_executable(runFluidSolverTests ${FluidSolverTests_SOURCE_FILES})

target_link_libraries(runFluidSolverTests gtest gtest_main gmock)
target_link_libraries(runFluidSolverTests FluidSolver)