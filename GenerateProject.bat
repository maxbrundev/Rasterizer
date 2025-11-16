@echo off
SET version=%~1%
if "%~1"=="" SET version="vs2022"

pushd Sources\OnyxEngine
call ..\..\Dependencies\premake5\bin\premake5.exe %version%
popd

PAUSE