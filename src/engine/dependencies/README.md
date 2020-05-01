## Automated dependency download

Use the given scripts `download_dependencies.sh` on UNIX systems or `download_dependencies.ps1` on windows systems to download and extract
the needed dependencies.

### Notes

This folder should contain GLFW-3.3 in a subdirectory called `GLFW`.
The directory structure should be the following:
```$xslt
dependencies
    GLFW
        include
        src
        tests
        ...
```
