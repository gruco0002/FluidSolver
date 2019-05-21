include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(FluidSolverTests_SOURCE_FILES
        basic_check.cpp
        CubicSplineKernelTest.cpp QuadraticNeighborhoodGreedyAllocatedTest.cpp QuadraticNeighborhoodDynamicAllocatedTest.cpp HelperFunctions.cpp HelperFunctions.hpp QuadraticNeighborhoodPreAllocatedTest.cpp HashedNeighborhoodSearchTest.cpp)

add_executable(runFluidSolverTests ${FluidSolverTests_SOURCE_FILES})

target_link_libraries(runFluidSolverTests gtest gtest_main)
target_link_libraries(runFluidSolverTests FluidSolver)