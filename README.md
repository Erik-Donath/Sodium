# Sodium
An small x86 Operating System which uses multiboot2 and the grub bootloader.

## Build
This project is build by using a docker container. Please install Docker.<br><br>
You first have to run 'docker build buildenv -t sodium-buildenv' to build the Docker Image
After building the buildenv run 'docker run --rm -it -v C:/Sodium/:/root/env sodium-buildenv' and replace 'C:/Sodium/' with the path to the root directory of this project. (Aka. the diretory path you are corently at.)

## Run
After building the Sodium you can emulate the system with qemu. Simply run 'qemu-system-x86_64 -cdrom dist/Sodium.iso'. You can also install the 'dist/Sodium.iso' with Rufus on an USB stick and Boot from it.

## Architekture
Here is an overview of the current project Architekture
<br>
Sodium/<br>
├── build<br>
├── src<br>
│   ├── kernel<br>
│   │   ├── kernel.c<br>
│   │   └── ...<br>
│   ├── boot<br>
│   │   ├── boot.asm<br>
│   │   ├── multiboot.asm<br>
│   │   └── grub<br>
│   │       └── grub.cfg<br>
├── include<br>
│   ├── kernel.h<br>
│   └── ...<br>
├── dist<br>
│   ├── Sodium.iso<br>
│   └── kernel.bin<br>
└── Makefile<br>
