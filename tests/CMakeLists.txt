project(FluidSolver_tests)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(dependencies/googletest)
add_subdirectory(FluidSolverTests)