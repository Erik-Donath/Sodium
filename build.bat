@echo off
if "%~1"=="1" (
    echo --^> Building Enviroment
    docker build buildenv -t sodium-buildenv
)
if "%errorlevel%" == "0" (
    echo --^> Building Sodium
    docker run --rm -it -v %cd%:/root/env sodium-buildenv
)