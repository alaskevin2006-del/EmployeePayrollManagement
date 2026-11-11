@echo off
setlocal
set "MSYS=C:\msys64\ucrt64"
set "PATH=%MSYS%\bin;%PATH%"
cd /d "%~dp0"

if not exist build mkdir build

"%MSYS%\bin\g++.exe" -std=c++17 -g -Wall -Wextra -I include main.cpp src\App.cpp src\Person.cpp src\Service.cpp src\UI.cpp -o build\app.exe -lgraphics -luuid -lmsimg32 -lgdi32 -limm32 -lole32 -loleaut32 -lwinmm -lgdiplus

if errorlevel 1 (
    echo.
    echo [BUILD FAILED]
    exit /b 1
)

echo.
echo [OK] build\app.exe
endlocal
