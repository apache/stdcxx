@echo off
set ERRORLEVEL=0
cscript /? > nul
if %errorlevel% neq 0 (
echo "Windows scripting host is not installed. Please install it."
set ERRORLEVEL=1
goto :EOF
)

set scriptdir=etc\config\windows
set rungen=generate.wsf

set runfile=%~dp0\%scriptdir%\%rungen%

cscript /nologo %runfile% %*
if %errorlevel% neq 0 (
echo "Solution generation script failed."
set ERRORLEVEL=1
goto :EOF
)
