# Fluid Solver
A small 2D SPH Fluid Solver implemented in C++ with OpenGL visualization. The fluid solver framework is tested with the help of googletest.
Furthermore it can be run as a command line tool to calculate fluid scenarios.

![Screenshot](https://github.com/gruco0002/FluidSolver/blob/master/resources/Screenshot.png?raw=true)

## Using the code
The whole project is cmake based and configured to run on established systems like common Linux distros, Windows machines
and OSX. The aim of the project is to run out of the box with as little work as possible.
Therefore no prebuilt binaries are needed, since cmake will compile each needed library.

### Downloading dependencies

To automatically download the needed dependencies and extract them into your folder execute the correct `download_dependencies` script
for your operating system in the repository root.
For UNIX based systems: `download_dependencies.sh`, for Windows: `download_dependencies.ps1`
Those scripts execute the individual download scripts contained in the repository.

There are currently two dependency folders in the project tree: `src/dependencies` and `tests/dependencies`.
Each folder contains a `README.md` and a download script that automatically downloads the needed dependencies, extracts them correctly
and cleans up afterwards. Those scripts can be called by yourself or will be started by calling the above described script
in the repository root folder.

Before working with this project or compiling it, you should download the dependencies by using the scripts or doing it manually.

#### Launching the scripts
On windows you can use the powershell scripts. Launch them by right clicking them in the file explorer and selecting
`Run with powershell`.
On Linux or MacOS make sure, that the scripts are executable (`chmod u+x download_dependencies.sh`) and launch them inside the dependency folder (the dependency
folder should be your current working directory).

#### Requirements for downloading
Make sure to have `git`, `wget` and `unzip` on UNIX based systems and that those tools are in your `PATH`.
Especially on Mac-OSX wget is not installed by default. You can obtain wget for OSX with `brew install wget`.

For Windows an updated version of `PowerShell` and `git` should be sufficient for downloading the dependencies.
Make sure that `git` is contained within your environment variables / path.

### Compiling
Only 64-bit compilation with the C++14 standard is supported. The project may work under other conditions but this is not tested.
Use CMake to compile the project:
- `FluidSolver_run` for an runnable executable
- `FluidSolver` for just the core code
- `runFluidSolverTests` to run the tests

### Used dependencies and libraries

- [GLFW](https://www.glfw.org/) (Version 3.3),
  *needs to be downloaded*
- [googletest](https://github.com/google/googletest) (Version 1.8.1),
  *needs to be downloaded*
- [GLAD](https://github.com/Dav1dde/glad) (Version 0.1.29),
  included in this repository
- [GLM](https://glm.g-truc.net/) (Version 0.9.7.1),
  included in this repository
- [stb_image](https://github.com/nothings/stb) (Version 2.19),
  included in this repository
- [stb_image_resize](https://github.com/nothings/stb) (Version 0.95),
  included in this repository
- [stb_image_write](https://github.com/nothings/stb) (Version 1.09),
  included in this repository
- [stb_truetype](https://github.com/nothings/stb) (Version 1.19),
  included in this repository
- OpenMP (Version 4.5)
- [cxxopts](https://github.com/jarro2783/cxxopts) (Version 2.1.2),
  included in this repository
- [Dear imgui](https://github.com/ocornut/imgui) (Version 1.74, docking branch),
  included in this repository
- [imgui-plot](https://github.com/soulthreads/imgui-plot) (Version ce5f24e),
  included in this repository

### Using the Code on OSX (Mac)
Since the project uses OpenMP (libomp) to parallelize the execution and the current
(21th May 2019) clang compiler that ships with OSX/XCode does not support OpenMP, you
will need to download a newer compiler version of clang by the llvm project.

The recommended way of downloading it is using brew: `brew install llvm`
Be sure that you have installed the XCode Command Line Tools for OSX before installing llvm.
To install those tools execute `xcode-select --install`.

After installation it could be possible, that you have to modify the `src/CMakeLists.txt`.
Below `if (APPLE)` are paths that have to be adapted to your llvm location. (Normally they should be `/usr/local/opt/llvm/`)
Please be sure that these paths behind the variable names `CMAKE_PREFIX_PATH`, `CMAKE_C_COMPILER`, `CMAKE_CXX_COMPILER` and `openmpDylibPath` are set correctly.
