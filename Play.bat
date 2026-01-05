@echo off
title Retro Adventure
cd /d "%~dp0"

if exist "build\Release\RetroAdventure.exe" (
    start "" "build\Release\RetroAdventure.exe"
) else (
    echo Game not built yet! Run Build.bat first.
    pause
)
