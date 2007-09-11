@echo off

set exedir=

if exist %SystemRoot%\SysWow64\cscript.exe set exedir=%SystemRoot%\SysWow64\

set ERRORLEVEL=0
%exedir%cscript /? > nul
if %errorlevel% neq 0 (
echo "Windows scripting host is not installed. Please install it."
set ERRORLEVEL=1
goto :EOF
)

set scriptdir=etc\config\windows
set rungen=generate.wsf

set runfile=%~dp0\%scriptdir%\%rungen%

%exedir%cscript /nologo %runfile% %*
if %errorlevel% neq 0 (
echo "Solution generation script failed."
set ERRORLEVEL=1
goto :EOF
)
