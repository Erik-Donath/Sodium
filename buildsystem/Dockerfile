FROM randomdude/gcc-cross-x86_64-elf:latest

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y gcc-multilib libc6-dev-i386 \
    nasm xorriso grub-pc-bin grub-common \
    cmake

VOLUME /workspace
WORKDIR /workspace

ENTRYPOINT ["bash", "/workspace/buildsystem/docker-entrypoint.sh"]