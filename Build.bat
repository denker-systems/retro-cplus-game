@echo off
title Build Retro Adventure
cd /d "%~dp0"

echo Building Retro Adventure...
echo.

cd build
cmake --build . --config Release

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful!
    echo Run Play.bat to start the game.
) else (
    echo.
    echo Build failed!
)

pause
