echo off
echo.

cd %~dp0\App
echo Cleaning App
rmdir /S /Q debug
rmdir /S /Q release
del .qmake.stash
del Makefile
del Makefile.Debug
del Makefile.Release
cd ..

cd WebApi
echo Cleaning WebApi
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

