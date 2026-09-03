@echo off
title K3N Armoni Composer - C++ Build & Run
echo =======================================================
echo   K3N ARMONI COMPOSER - NATIVE C++ BUILD & RUN
echo =======================================================
echo.

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo [ATTENZIONE] CMake non e' installato o non e' nel PATH.
    echo Per provare subito l'interfaccia interattiva e i suoni senza compilare,
    echo fai doppio click sul file: AVVIA_SIMULATORE.bat
    echo.
    echo Per compilare il file .exe nativo in C++:
    echo 1. Installa CMake da https://cmake.org/download/
    echo 2. Installa Visual Studio Community con supporto C++
    echo.
    pause
    exit /b 1
)

if not exist "%~dp0JUCE" (
    echo [1/3] Scaricamento sottomodulo JUCE Framework in corso...
    git clone --depth 1 https://github.com/juce-framework/JUCE.git "%~dp0JUCE"
)

echo [2/3] Configurazione CMake in corso...
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo [3/3] Compilazione eseguibile LoopStation.exe in corso...
cmake --build build --config Release

if exist "%~dp0build\LoopStation_artefacts\Release\LoopStation.exe" (
    echo.
    echo [OK] Compilazione completata con successo!
    echo Avvio di LoopStation.exe...
    start "" "%~dp0build\LoopStation_artefacts\Release\LoopStation.exe"
) else (
    echo [ERRORE] File eseguibile non trovato dopo la compilazione.
)

pause
