echo on

cd %~dp0

set RUN_MODE=%1

set BUILD_TYPE=%2
set BUILD_BITS=%3
set BUILD_COMPILE=%4

if "%RUN_MODE%"=="" (set RUN_MODE=man)

if "%RUN_MODE%"=="msvc"  (call script\build_msvc.bat  %BUILD_TYPE% %BUILD_BITS% %BUILD_COMPILE%)
if "%RUN_MODE%"=="mingw" (call script\build_mingw.bat %BUILD_TYPE% %BUILD_BITS% %BUILD_COMPILE%)

rem run by default or double click
if "%RUN_MODE%"=="man" (call script\build_msvc.bat Debug x64 VS2015)

rem run by ci or full release build
if "%RUN_MODE%"=="ci" (call script\ci.bat)

if "%RUN_MODE%"=="clean" (call script\clean.bat)
if "%RUN_MODE%"=="config" (call script\config.bat)

rem pause when run build script manually
if not "%RUN_MODE%"=="ci" (pause)
