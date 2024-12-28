@echo off
if "%~1"=="1" call build.bat 0
if "%~1"=="2" call build.bat 1
if "%errorlevel%" == "0" (
    echo --^> Running
    qemu-system-x86_64 -debugcon stdio -cdrom dist/Sodium.iso
)