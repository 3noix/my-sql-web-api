echo off

set EXECUTABLE=%~dp0\SimpleHttpServer\release\SimpleHttpServer.exe
set RESOURCESDIR=%~dp0\AppWeb

%EXECUTABLE% %RESOURCESDIR%

echo.
pause

