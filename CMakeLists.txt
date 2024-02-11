if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there")
endif ()
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0003 NEW)
IF(POLICY CMP0012)
    CMAKE_POLICY(SET CMP0012 NEW)
ENDIF()

cmake_minimum_required(VERSION 3.13)
project(FluidSolver VERSION 1)


if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "32 bit isn't supported")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
add_subdirectory(tests)

enable_testing()
add_test(UnitTests runUnitTests)
