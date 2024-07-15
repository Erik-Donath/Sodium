# My OS

## Setup build env
Download docker and run: 'docker build buildenv -t myos-buildenv'

## Build
After building the buildenv run 'docker run --rm -it -v C:/MyOs/:/root/env myos-buildenv' and replace 'C:/MyOs/' with the current path.

## Run
After building the os in the buildenv run 'qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso'
