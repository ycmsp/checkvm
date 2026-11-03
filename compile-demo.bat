@echo off
setlocal enabledelayedexpansion

cls
echo ========================================
echo Compiling CheckVM Demo (console EXE)
echo ========================================
echo.

set OPENSSL_CONF=

if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
    set VCVARSALL_PATH="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
    set VCVARSALL_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    set VCVARSALL_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
    set VCVARSALL_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
)

if not defined VCVARSALL_PATH (
    echo ERROR: VS 2022 not found.
    exit /b 1
)

call !VCVARSALL_PATH! x64
if errorlevel 1 exit /b 1

del /f /q demo.exe demo.obj >nul 2>&1

echo Compiling demo.cpp...
cl.exe /EHsc /W3 /nologo /std:c++17 /O2 /utf-8 /D _CRT_SECURE_NO_WARNINGS demo.cpp /link /SUBSYSTEM:CONSOLE /MACHINE:x64 /OUT:demo.exe

if errorlevel 1 (
    echo ERROR: Compilation failed.
    exit /b 1
)

echo.
echo ========================================
echo Compilation successful!  demo.exe ready
echo ========================================
echo.
echo Make sure checkvm.dll is in the same directory.
echo.
demo.exe