if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there")
endif ()
cmake_policy(SET CMP0048 NEW)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(FluidSolver VERSION 1)
cmake_minimum_required(VERSION 3.13)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "32 bit isn't supported")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(tests)
