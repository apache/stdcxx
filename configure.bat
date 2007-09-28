@echo off

::
:: Licensed to the Apache Software  Foundation (ASF) under one or more
:: contributor  license agreements.  See  the NOTICE  file distributed
:: with  this  work  for  additional information  regarding  copyright
:: ownership.   The ASF  licenses this  file to  you under  the Apache
:: License, Version  2.0 (the  "License"); you may  not use  this file
:: except in  compliance with the License.   You may obtain  a copy of
:: the License at
::
:: http://www.apache.org/licenses/LICENSE-2.0
::
:: Unless required by applicable law or agreed to in writing, software
:: distributed under the  License is distributed on an  "AS IS" BASIS,
:: WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
:: implied.   See  the License  for  the  specific language  governing
:: permissions and limitations under the License.
::
:: Copyright 1999-2007 Rogue Wave Software, Inc.
::

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
