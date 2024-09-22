# Sodium
An small x86 Operating System which uses multiboot2 and the grub bootloader.<br>
Warning: This Sytem is still in early development. Many things can (and propatly will) go wrong.

## Build & Run
This project is build by using a Docker container. <br><br>
Steps to <b>Build and Run</b>:<br>
<b>1:</b> Install Docker (Linux: ```apt install docker```, Windows: ```winget install Docker.DockerDesktop```)<br>
<b>2:</b> Optional: Install qemu (Linux: ```apt install qemu```, Windows: visit 'https://www.qemu.org/' and use 'Stefan Weil' binaries.<br>
<b>3:</b> Run ```docker build buildenv -t sodium-buildenv``` to build the builenv<br>
<b>4:</b> Use ```docker run --rm -it -v C:/Sodium/:/root/env sodium-buildenv``` and replace 'C:/Sodium/' with the project directory<br>
<b>5:</b> <b>Run</b> ```qemu-system-x86_64 -debugcon stdio -cdrom dist/Sodium.iso``` to start the OS! (Only if qemu is install)<br>

You can also run the OS by using the .iso file directly (For example in VBox or on a LiveUSB).

## Architekture
Here is an overview of the current project Architekture
```
Sodium
├── buildenv
│   └── Dockerfile
├── script
│   └── Code Generation Scripts (used by Makefile)
├── src
│   ├── kernel
│   │   ├── arch/i686
│   │   │   └── / i686 Driver and Systems /
│   │   ├── hal
│   │   │   └── / Hardware Abstraction Layer /
│   │   ├── util
│   │   │   └── / Universal Stuff /
│   │   ├── kernel.c
│   │   └── ...
│   ├── boot
│   │   ├── boot.asm
│   │   ├── multiboot.asm
│   │   └── grub
│   │       └── grub.cfg
│   └── linker.ld
├── dist
│   ├── Sodium.iso
│   └── kernel.bin
├── build
│   └── Build Directory
├── Makefile
├── README.md
└── LICENSE
```
