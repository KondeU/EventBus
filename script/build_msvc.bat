cd %~dp0..

set BUILD_TYPE=%1
set BUILD_BITS=%2
set BUILD_COMPILE=%3

rem script params: msvc.bat [BUILD_TYPE] [BUILD_COMPILE] [BUILD_BITS]
rem default value: BUILD_TYPE=Debug BUILD_COMPILE=VS2017 BUILD_BITS=x86

if "%BUILD_TYPE%"=="" (set BUILD_TYPE=Debug)
if "%BUILD_COMPILE%"=="" (set BUILD_COMPILE=VS2017)

set BUILD_COMPILE_NUM=%BUILD_COMPILE:~-2%

if "%BUILD_COMPILE%"=="VS2015" (set BUILD_COMPILE=Visual Studio 14 2015)
if "%BUILD_COMPILE%"=="VS2017" (set BUILD_COMPILE=Visual Studio 15 2017)
if "%BUILD_COMPILE%"=="VS2019" (set BUILD_COMPILE=Visual Studio 16 2019)
if "%BUILD_COMPILE%"=="VS2022" (set BUILD_COMPILE=Visual Studio 17 2022)

rem over VS2019 uses "BUILD_ARCH_ARG"
set BUILD_ARCH_ARG=
if %BUILD_COMPILE_NUM% GEQ 19 (
if "%BUILD_BITS%"=="x64" (set BUILD_ARCH_ARG=-A x64) ^
else (set BUILD_ARCH_ARG=-A Win32)
) else (
if "%BUILD_BITS%"=="x64" (set BUILD_COMPILE=%BUILD_COMPILE% Win64)
)

if exist "build\dependency" (goto _tag_build_trace_src)

:_tag_build_trace_3rd
rem ===== build trace <3rd> =====

mkdir build\build_3rd\zeromq
cd build\build_3rd\zeromq

mkdir zeromq
cd zeromq
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ^
-DBUILD_SHARED=ON ^
-DBUILD_STATIC=OFF ^
-DBUILD_TESTS=OFF ^
-DZMQ_BUILD_TESTS=OFF ^
-DCMAKE_INSTALL_PREFIX=%~dp0..\build\dependency\zeromq\zeromq ^
-DWITH_LIBSODIUM=OFF ^
../../../../3rd/zeromq
cmake --build . --config %BUILD_TYPE% -j 8
cmake --install . --config %BUILD_TYPE%
cd ..

mkdir cppzmq
cd cppzmq
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ^
-DCPPZMQ_BUILD_TESTS=OFF ^
-DCMAKE_INSTALL_PREFIX=%~dp0..\build\dependency\zeromq\cppzmq ^
-DZeroMQ_DIR=%~dp0..\build\dependency\zeromq\zeromq\CMake ^
../../../../3rd/cppzmq
cmake --build . --config %BUILD_TYPE% -j 8
cmake --install . --config %BUILD_TYPE%
cd ..

cd ..\..\..

mkdir build\dependency\cereal
cd build\dependency\cereal
mkdir include
xcopy %~dp0..\3rd\cereal\include include /E /Y
cd ..\..\..

mkdir build\dependency\msgpack
cd build\dependency\msgpack
mkdir include
xcopy %~dp0..\3rd\msgpack\include include /E /Y
cd ..\..\..

:_tag_build_trace_src
rem ===== build trace <src> =====

mkdir build\build_src
cd build\build_src
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ../../src
cmake --build . --config %BUILD_TYPE% -j 8
cd ..\..

:_tag_archive
rem ===== archive =====

mkdir build\archive
cd build\archive
cd ..\..
