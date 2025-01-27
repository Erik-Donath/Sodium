# Sodium
An small x86 Operating System which uses multiboot2 and the grub bootloader.<br>
Warning: This Sytem is still in early development. Many things can (and propatly will) go wrong.

# What is a Operating System
![EX1](EX1.png)

## Build & Run
This project is build by using a Docker container. <br><br>
Steps to <b>Build and Run</b>:<br>
<b>1:</b> Install Docker (Linux: ```apt install docker```, Windows: ```winget install Docker.DockerDesktop```)<br>
<b>2:</b> Optional: Install qemu (Linux: ```apt install qemu```, Windows: visit 'https://www.qemu.org/' and use 'Stefan Weil' binaries.<br>
<b>3:</b> Run ```docker build buildenv -t sodium-buildenv``` to build the builenv<br>
<b>4:</b> Windows: Use ```docker run --rm -it -v C:/Sodium/:/root/env sodium-buildenv``` and replace 'C:/Sodium/' with the project directory<br>
<b>4:</b> Linux: Use ```sudo docker run --rm -it -v ./:/root/env:Z sodium-buildenv```<br>
<b>5:</b> <b>Run</b> ```qemu-system-x86_64 -debugcon stdio -cdrom dist/Sodium.iso``` to start the OS! (Only if qemu is install)<br>

You can also run the OS by using the .iso file directly (For example in VBox or on a LiveUSB).

## Architekture
Here is an overview of the current project Architekture
```
Sodium
├── buildenv
│   ├── docker-entrypoint.sh
│   └── Dockerfile
├── src
│   ├── kernel
│   │   ├── arch/i686
│   │   │   └── / i686 Driver and Systems /
│   │   ├── kernel.c
│   │   └── ...
│   ├── boot/i686
│   │   ├── boot.asm
│   │   ├── multiboot.inc
│   │   ├── shutdown.asm
│   │   └── grub
│   │       └── grub.cfg
│   └── linker.ld
├── dist
│   ├── Sodium.iso
│   └── kernel.bin
├── build
│   └── /* Build Directory */
├── Makefile
├── README.md
└── LICENSE
```
