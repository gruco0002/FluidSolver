include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR})

set(FluidSolverTests_SOURCE_FILES
        basic_check.cpp
        CubicSplineKernelTest.cpp NeighborhoodSearchTests.cpp CompactHashingComponentTests/CompactHashingCellStorageTests.cpp CompactHashingComponentTests/CompactHashingHashTableTests.cpp)

add_executable(runFluidSolverTests ${FluidSolverTests_SOURCE_FILES})

target_link_libraries(runFluidSolverTests PUBLIC FluidSolver GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)