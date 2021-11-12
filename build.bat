echo off
echo.

cd %~dp0\WebApi
echo Building WebApi
echo.
qmake
mingw32-make release
cd ..
echo.

cd App
echo Building App
echo.
qmake
mingw32-make release
cd ..
echo.

cd bin
echo Deploying binaries
echo.
windeployqt WebApi.exe
windeployqt App.exe
copy /Y ..\Database\MySQL-Connector-C-6.1-x86\libmysql.dll .
cd ..
echo.

echo.
echo Press a key to terminate
pause
