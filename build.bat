echo off
echo.

cd %~dp0\web-api
echo Building web-api
echo.
qmake
mingw32-make release
cd ..
echo.

cd app-qt
echo Building app-qt
echo.
qmake
mingw32-make release
cd ..
echo.

cd bin
echo Deploying binaries
echo.
windeployqt web-api.exe
windeployqt app-qt.exe
copy /Y ..\Database\MySQL-Connector-C-6.1-x86\libmysql.dll .
cd ..
echo.

echo.
echo Press a key to terminate
pause
