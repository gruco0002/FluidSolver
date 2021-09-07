find_package(cxxopts CONFIG REQUIRED)

# include directories
include_directories(./)
include_directories(../libFluid)


set(FLUID_CONSOLE_SOURCE_FILES
        "main.cpp"
         
        )


add_executable(FluidConsole ${FLUID_CONSOLE_SOURCE_FILES})
target_link_libraries(FluidConsole PUBLIC cxxopts::cxxopts)
target_link_libraries(FluidConsole PUBLIC libFluid)

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FLUID_CONSOLE_SOURCE_FILES})

