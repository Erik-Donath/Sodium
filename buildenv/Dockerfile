FROM randomdude/gcc-cross-x86_64-elf:latest

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y gcc-multilib libc6-dev-i386
RUN apt-get install -y nasm xorriso grub-pc-bin grub-common
#RUN apt-get install -y dos2unix

VOLUME /root/env
WORKDIR /root/env

ENTRYPOINT [ "make", "info", "all" ]
