echo off
echo.

cd %~dp0\app-qt
echo Cleaning app-qt
rmdir /S /Q debug
rmdir /S /Q release
del .qmake.stash
del Makefile
del Makefile.Debug
del Makefile.Release
cd ..

cd web-api
echo Cleaning web-api
rmdir /S /Q debug
rmdir /S /Q release
del .qmake.stash
del Makefile
del Makefile.Debug
del Makefile.Release
cd ..

echo Deleting bin directory
rmdir /S /Q bin

echo.
echo cleaning finished
pause

