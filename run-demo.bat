@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
cd /d "%~dp0"
echo === CheckVM Demo Test ===
echo.
demo.exe
echo.
echo === Exit code: %ERRORLEVEL% ===
pause