echo off

set EXECUTABLE=%~dp0\simple-http-server\release\simple-http-server.exe
set RESOURCESDIR=%~dp0\app-web

%EXECUTABLE% %RESOURCESDIR%

echo.
pause

