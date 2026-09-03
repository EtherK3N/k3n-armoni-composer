@echo off
title K3N Armoni Composer - Docker Isolated Build & Test
echo =======================================================
echo   K3N ARMONI COMPOSER - DOCKER ISOLATED TEST RUNNER
echo =======================================================
echo.

where docker >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERRORE] Docker non e' in esecuzione o non e' installato nel PATH.
    echo Avvia Docker Desktop e riprova.
    pause
    exit /b 1
)

echo [1/2] Costruzione dell'immagine Docker isolata (k3n-armoni-tester)...
docker build -t k3n-armoni-tester .

if %errorlevel% neq 0 (
    echo [ERRORE] Errore durante la compilazione nel container Docker.
    pause
    exit /b 1
)

echo.
echo [2/2] Esecuzione della Test Suite automatizzata all'interno di Docker...
echo.
docker run --rm k3n-armoni-tester

echo.
echo =======================================================
echo   TEST COMPLETATI CON SUCCESSO DENTRO IL CONTAINER!
echo =======================================================
echo.
pause
