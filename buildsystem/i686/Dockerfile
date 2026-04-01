FROM debian:bookworm-slim

# Build dependencies for compiling binutils + GCC from source,
# and tools needed for the kernel build itself.
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    libgmp-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    cmake \
    && rm -rf /var/lib/apt/lists/*

ENV TARGET=i686-elf
ENV PREFIX=/usr/local
ENV PATH="$PREFIX/bin:$PATH"

ENV BINUTILS_VERSION=2.41
ENV GCC_VERSION=13.2.0

# Binutils
RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz \
    && tar xf binutils-${BINUTILS_VERSION}.tar.gz \
    && mkdir build-binutils \
    && cd build-binutils \
    && ../binutils-${BINUTILS_VERSION}/configure \
    --target=${TARGET} \
    --prefix=${PREFIX} \
    --disable-nls \
    --disable-werror \
    && make -j$(nproc) \
    && make install \
    && cd / && rm -rf binutils-${BINUTILS_VERSION} build-binutils binutils-${BINUTILS_VERSION}.tar.gz

# Build GCC
RUN wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz \
    && tar xf gcc-${GCC_VERSION}.tar.gz \
    && mkdir build-gcc \
    && cd build-gcc \
    && ../gcc-${GCC_VERSION}/configure \
    --target=${TARGET} \
    --prefix=${PREFIX} \
    --disable-nls \
    --enable-languages=c \
    --without-headers \
    && make -j$(nproc) all-gcc \
    && make -j$(nproc) all-target-libgcc \
    && make install-gcc \
    && make install-target-libgcc \
    && cd / && rm -rf gcc-${GCC_VERSION} build-gcc gcc-${GCC_VERSION}.tar.gz

VOLUME /workspace
WORKDIR /workspace

ENTRYPOINT ["bash", "/workspace/buildsystem/i686/docker-entrypoint.sh"]