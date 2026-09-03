@echo off
title K3N Armoni Composer - Native C++ Build & Run
echo =======================================================
echo   K3N ARMONI COMPOSER - NATIVE C++ BUILD & RUN
echo =======================================================
echo.

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [WARNING] CMake is not installed or not in system PATH.
    echo To test the interactive interface and sounds immediately without building:
    echo Double-click on: LAUNCH_SIMULATOR.bat
    echo.
    echo To build the native C++ executable:
    echo 1. Install CMake from https://cmake.org/download/
    echo 2. Install Visual Studio Community with C++ workload
    echo.
    pause
    exit /b 1
)

if not exist "%~dp0JUCE" (
    echo [1/3] Cloning JUCE Framework submodule...
    git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git "%~dp0JUCE"
)

echo [2/3] Configuring CMake project...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo [3/3] Compiling LoopStation.exe binary...
cmake --build build --config Release --parallel

if exist "%~dp0build\LoopStation_artefacts\Release\LoopStation.exe" (
    echo.
    echo [OK] Build completed successfully!
    echo Launching LoopStation.exe...
    start "" "%~dp0build\LoopStation_artefacts\Release\LoopStation.exe"
) else (
    echo [ERROR] Executable binary not found after build.
)

pause
