@echo off

echo Building project...

gcc main.c garbage_collector.c monitor.c -o main.exe

if %errorlevel% neq 0 (
    echo.
    echo Build failed!
    exit /b %errorlevel%
)

echo Build successful!
echo.
echo Starting GC...
echo.

main.exe