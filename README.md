# Sodium

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A small, hobbyist-grade x86 operating system which uses Multiboot2 with the GRUB
bootloader.

## Structure of the project

```txt
.
├── buildsystem
│   ├── cmake
│   │   └── i686
│   │       └── CMakeLists.txt
│   ├── docker-entrypoint.sh
│   └── Dockerfile
├── LICENSE
├── Makefile
├── README.md
└── src
    ├── arch
    │   └── i686
    │       ├── gdt.asm
    │       ├── gdt.c
    │       ├── gdt.h
    │       ├── io.h
    │       ├── pre_kernel.c
    │       └── pre_kernel.h
    ├── bootloader
    │   └── i686
    │       ├── boot.asm
    │       ├── grub
    │       │   └── grub.cfg
    │       ├── linker.ld
    │       └── shutdown.asm
    ├── kernel
    └── libk

12 directories, 16 files
```
