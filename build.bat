@echo off
if "%~1"=="1" docker build buildenv -t sodium-buildenv
if "%errorlevel%" == "0" docker run --rm -it -v %cd%:/root/env sodium-buildenv
