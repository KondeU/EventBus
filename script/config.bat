:: Manually configure the local environment.

PATH=D:\CMake\cmake-3.16.0-win64-x64\bin;%PATH%
PATH=D:\CMake\cmake-3.19.8-win64-x64\bin;%PATH%
PATH=D:\CMake\cmake-3.22.0-windows-x86_64\bin;%PATH%

echo Env PATH: %PATH%

:: The version of CMake finally used:
cmake --version

:: The default build uses VS2017, if we have a higher version of VS,
:: use the higher version of MSBuild to build, then we need to add
:: the path of MSBuild.exe to the environment PATH.
:: And we need to install compatible compile and build tools(Windows SDK).
PATH=D:\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin;%PATH%
