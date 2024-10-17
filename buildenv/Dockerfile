FROM randomdude/gcc-cross-x86_64-elf:latest

# Install nesesary components
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y nasm xorriso grub-pc-bin grub-common
RUN apt-get install -y dos2unix

# You have to bind '/root/env' to the project folder!
VOLUME /root/env
WORKDIR /root/env

# Copies the docker-entrypoint file into the Docker container
# And preprares it for executing it on Linux.
COPY docker-entrypoint.sh /usr/local/bin/
RUN dos2unix /usr/local/bin/docker-entrypoint.sh
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# Will build the Project automatikly
ENTRYPOINT [ "/usr/local/bin/docker-entrypoint.sh"]
