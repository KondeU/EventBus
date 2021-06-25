set PATCHES_PATH="%~dp0patches"
set OPEN_SOURCE_PATH="%~dp0..\3rd"

cd %OPEN_SOURCE_PATH%
powershell -executionpolicy remotesigned -file "%~dp0extract_open_source.ps1"
