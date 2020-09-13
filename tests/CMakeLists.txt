project(FluidSolver_tests)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(GOOGLETEST_VERSION 1.10.0)

find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)
add_subdirectory(FluidSolverTests)