@echo off
if "%~1"=="1" docker build buildenv -t sodium-buildenv
docker run --rm -it -v %cd%:/root/env sodium-buildenv