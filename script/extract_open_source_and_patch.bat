set PATCHES_PATH=%~dp0patches
set OPEN_SOURCE_PATH=%~dp0..\3rd

cd %OPEN_SOURCE_PATH%
powershell -executionpolicy remotesigned -file "%~dp0extract_open_source.ps1"

cd %OPEN_SOURCE_PATH%\zeromq
%OPEN_SOURCE_PATH%\patch\bin\patch.exe -p1 < %PATCHES_PATH%\0001.patch

cd %OPEN_SOURCE_PATH%\cppzmq
%OPEN_SOURCE_PATH%\patch\bin\patch.exe -p1 < %PATCHES_PATH%\0002.patch

cd %~dp0..
