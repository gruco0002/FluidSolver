## Automated dependency download

Use the given scripts `download_dependencies.sh` on UNIX systems or `download_dependencies.ps1` on windows systems to download and extract
the needed dependencies.


### Notes

This folder should contain the googletest code in a subdirectory called `googletest`.
The directory structure should be the following:
```$xslt
dependencies
    googletest-1.10.0
        googlemock
        googletest
        ...
```

Please download googletest v1.10.0 from https://github.com/google/googletest/releases
or use the automated dependency download
