# Fluid Solver
A small 2D SPH Fluid Solver implemented in C++ with OpenGL visualization. The fluid solver framework is tested with the
help of googletest.

## Using the code
The whole project is cmake based and configured to run on established systems like common Linux distros, Windows machines
and OSX. The aim of the project is to run out of the box with as little work as possible.
Therefore no prebuilt binaries are needed, since cmake will compile each needed library.

### Downloading dependencies
There are currently two dependency folders in the project tree: `src/dependencies` and `tests/dependencies`. Each folder
contains a `README.md` and a download script that automatically downloads the needed dependencies, extracts them correctly
and cleans up afterwards.
Before working with this project or compiling it, you should download the dependencies by using the scripts or doing it manually.

#### Launching the scripts
On windows you can use the powershell scripts. Launch them by right clicking them in the file explorer and selecting
`Run with powershell`.
On Linux or MacOS make sure, that the scripts are executable (`chmod u+x download_dependencies.sh`) and launch them inside the dependency folder (the dependency
folder should be your current working directory).

### Compiling
Only 64-bit compilation with the C++14 standard is supported. The project may work under other conditions but this is not tested.
Use CMake to compile the project:
- `FluidSolver_run` for an runnable executable
- `FluidSolver` for just the core code
- `runFluidSolverTests` to run the tests

### Used dependencies and libraries

- GLFW (Version 3.3), needs to be downloaded
- googletest (Version 1.8.1), needs to be downloaded
- GLAD (Version 0.1.29), included in this repository
- GLM (Version 0.9.7.1), included in this repository
- stb_image (Version 2.19), included in this repository
- stb_image_resize (Version 0.95), included in this repository
- stb_image_write (Version 1.09), included in this repository
- stb_truetype (Version 1.19), included in this repository
