## Automated dependency download

Use the given scripts `download_dependencies.sh` on UNIX systems or `download_dependencies.ps1` on windows systems to download and extract
the needed dependencies.

### Notes
Since GLAD is very small, it is already contained in the repo.

This folder should contain glad in a subdirectory called `Glad`, GLFW-3.3 in a
subdirectory called `GLFW` and cppgui in a subdirectory called `cppgui`.
The directory structure should be the following:
```$xslt
dependencies
    Glad
        glad
        KHR
        glad.c
        CMakeLists.txt
        ...
    GLFW
        include
        src
        tests
        ...
    cppgui
        src
        ...
```
