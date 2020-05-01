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

There are currently three dependency folders in the project tree: `src/dependencies`, `src/engine/dependencies` and `tests/dependencies`.
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
Only 64-bit compilation with the C++17 standard is supported. The project may work under other conditions but this is not tested.
Use CMake to compile the project:
- `FluidSolver_run` for an runnable executable
- `FluidSolver` for just the core code
- `runFluidSolverTests` to run the tests

### Used dependencies and libraries

- [GLFW](https://www.glfw.org/) (Version 3.3),
  *needs to be downloaded*
- [googletest](https://github.com/google/googletest) (Version 1.10.0),
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
- [Dear imgui](https://github.com/ocornut/imgui) (Version 1.76, docking branch),
  included in this repository
- [imgui-plot](https://github.com/soulthreads/imgui-plot) (Version ce5f24e),
  included in this repository
- [ImGuiFileDialog](https://github.com/aiekick/ImGuiFileDialog) (Version 2f339cc),
  included in this repository
- [libmorton](https://github.com/Forceflow/libmorton) (Version v0.2.2),
  *needs to be downloaded*
  
#### Disclaimer
The listed libraries are not my own and i take no responsibilities for their content.
Some of those libraries are only included in the repository to provide an easy entry
point for someone that wants to checkout the repository and this fluid solver.

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

### Using the Code on Linux (Debian / Ubuntu)
The project uses OpenGL for its visualization. Therefore you have to install required
libraries in order to compile this project:
```shell script
sudo apt install mesa-utils
sudo apt install libgl1-mesa-dev
sudo apt install xorg-dev
```
*Important:* The packages could have other names for your distibution of linux. Also the
package manager (here `apt`) is not the same for every linux system.

The mesa packages are required for OpenGL support. The xorg-dev is required in order to
create a window in which OpenGL can render its stuff.
Also make sure to have a decent c++ and c compiler like gcc available on your system.
I recommend installing the build-essential package, since it contains the compiler and
other important stuff:
```shell script
sudo apt-get install build-essential
```
To create a CMake config you need to have CMake installed. You can obtain cmake by installing
it with your package manager. A good tool to set up the project would be VS Code
(by Microsoft) with the CMake extension (by Microsoft) and C/C++ extension (by Microsoft)
installed. VS Code requires CMake to be installed on the system. 
An alternative would be to use tools like CLion that automatically create the CMake
config for you. Clion ships with its own version of CMake but requires compilers and
libraries to be installed on the system by you.

*Important:* Make sure your compiler supports a decent version of OpenMP. (Most modern
compilers for linux (like gcc) have that built in)

*Important:* GLM is a library that is included in the repository. With some compilers
it creates errors, since it does not recognize the compiler version.
If a cryptic glm error occurs while compiling the project, head to the
`src/libraries/glm/simd/platform.h` file. In this file you will find compiler version
checks from line `150` and onwards. It could be the case, that your compiler has a
version that is not explicitly captured by the checks. Check your compiler type, name
and version and add a line that defines the appropriate value for `GLM_COMPILER`.
