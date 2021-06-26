:: The comment use the '::' will not be displayed in
:: the terminal, so use rem to comment if run as ci.

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x64_debug
call build_msvc.bat Debug x64 VS2015
xcopy build\archive artifact\msvc_x64_debug /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x64_release
call build_msvc.bat Release x64 VS2015
xcopy build\archive artifact\msvc_x64_release /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x86_debug
call build_msvc.bat Debug x86 VS2015
xcopy build\archive artifact\msvc_x86_debug /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
mkdir ..\artifact\msvc_x86_release
call build_msvc.bat Release x86 VS2015
xcopy build\archive artifact\msvc_x86_release /E /Y
rem build_msvc will change the working path to .\..

cd %~dp0
call clean.bat
call build_mingw.bat
rem build_mingw will change the working path to .\..
