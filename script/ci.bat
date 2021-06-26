:: The comment use the '::' will not be displayed in
:: the terminal, so use rem to comment if run as ci.

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x64
call build_msvc.bat Release x64 VS2015
xcopy build\archive artifact\msvc_x64 /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x86
call build_msvc.bat Release x86 VS2015
xcopy build\archive artifact\msvc_x86 /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
call build_mingw.bat
rem build_mingw will change the working path to .\..
