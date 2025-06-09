# Sodium

An small, hobbyist-grade  x86 operating  system which uses  Multiboot2
with the GRUB bootloader.

> Warning: This sytem  is still in early development.  Many things can
> (and probably will) go wrong.

## What is an Operating System

![EX1](EX1.png)

## Building on Windows (Release 10 & 11)

Visit [QEMU's website](https://www.qemu.org/) and install the binaries
by *Stefan Weil*.

Open a command-line and install the  Docker containerization system by
running the command:

```
winget install Docker.DockerDesktop
```

To  build the operating  system, the following  command  should be run
*but* `PROJECT_PATH` should  be replaced with  the full, absolute path
at which the project resides.

```
docker run --rm -it -v PROJECT_PATH:/root/env sodium-buildenv
```

## Building on Debian Linux

Install Docker by running the following command as super-user:

```Bash
apt install docker qemu
```

Create the build environment  container by running the following (with
super-user permissions):

```Bash
docker buildx build buildenv -t sodium-os:buildenv
```

Build the project using the newly-created container with:

```Bash
sudo docker run --rm -it -v ./:/root/env:Z sodium-buildenv
```

## Running

If no error was  produced by the previous command, it  now is possible
to run the operating system using:

```Bash
qemu-system-x86_64 -debugcon stdio -cdrom dist/Sodium.iso
```

You can also run the OS by using the `.iso` file directly (For example
in VirtualBox or on a flash drive).

## Folder Structure

The following is an overview of Sodium's current folder structure:

- `buildenv/`
  - `docker-entrypoint.sh`
  - `Dockerfile`
- `src/`
  - `kernel/`  
    Contains all sources of the kernel and its drivers.
    - `arch/i686/`
      - *i686 Driver and Systems*
  - `boot/i686/`
    - `boot.asm`
    - `multiboot.inc`
    - `shutdown.asm`
    - `grub/`
      - `grub.cfg`
  - `linker.ld`
- `dist/`  
  Contains the output file; the file produced by the build script.
  - `Sodium.iso`
- `kernel.bin`
- `build/`  
  Contains intermediate (object code) files used to build the system.
- `Makefile`
- `README.md`
- `LICENSE`

