FROM randomdude/gcc-cross-x86_64-elf:latest

# Install nesesary components
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y nasm xorriso grub-pc-bin grub-common

# You have to bind '/root/env' to the project folder!
VOLUME /root/env
WORKDIR /root/env

# Will build the Project automatikly
ENTRYPOINT [ "make", "all" ]
