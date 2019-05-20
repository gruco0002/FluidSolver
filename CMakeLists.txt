if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there")
endif ()

project(FluidSolver)
cmake_minimum_required(VERSION 3.13)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "32 bit isn't supported")
endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
if (MINGW)
    set(CMKAE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -pthread")
endif ()

# include directories

include_directories(src)
include_directories(src/dependencies)
include_directories(src/dependencies/Glad)

add_subdirectory(src)
add_subdirectory(tests)



add_executable(FluidSolver_run main.cpp)
target_link_libraries(FluidSolver_run FluidSolver)