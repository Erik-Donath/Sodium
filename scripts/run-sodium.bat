@echo off
setlocal enabledelayedexpansion

echo Sodium OS Runner
echo ====================

echo Building Sodium...
docker run --rm -v %cd%:/root/env:Z sodium-buildenv

if not exist "dist\Sodium.iso" (
    echo Error: Sodium.iso not found!
    pause
    exit /b 1
)

where qemu-system-x86_64 >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: qemu-system-x86_64 not found!
    echo Please install QEMU to run Sodium.
    echo Visit: https://www.qemu.org/
    pause
    exit /b 1
)

echo Starting Sodium in QEMU...
echo Press Ctrl+Alt+G to release mouse capture
echo Press Ctrl+Alt+2 to access QEMU monitor
echo.

qemu-system-x86_64 ^
    -debugcon stdio ^
    -cdrom dist\Sodium.iso ^
    -m 128M ^
    -display gtk ^
    -name "Sodium OS"

pause
