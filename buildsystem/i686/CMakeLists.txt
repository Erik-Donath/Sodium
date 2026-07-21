cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")
set(CMAKE_ASM_NASM_FLAGS " -I${SRC}")
set(CMAKE_ASM_NASM_FLAGS_DEBUG "-g -F dwarf -DSODIUM_DEBUG")
set(CMAKE_ASM_NASM_FLAGS_RELEASE "")

project(Sodium C ASM_NASM)

set(SRC ${SRC} CACHE PATH "Path to src/ directory")
set(DIST /workspace/dist)

set(CMAKE_C_COMPILER i686-elf-gcc)
set(CMAKE_ASM_NASM_COMPILER nasm)

# Compiler flags
set(BASE_FLAGS "-ffreestanding -std=c11 -nostartfiles -nodefaultlibs -Wall -Wextra -Wa,--noexecstack")
set(CMAKE_C_FLAGS "${BASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g -DSODIUM_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Package: bootloader
file(GLOB BOOTLOADER_SOURCES
    ${SRC}/bootloader/i686/*.asm
)
add_library(bootloader OBJECT ${BOOTLOADER_SOURCES})

# Package: kernel
file(GLOB_RECURSE KERNEL_C_SOURCES
    ${SRC}/arch/i686/*.c
    ${SRC}/libk/*.c
    ${SRC}/kernel/*.c
)
file(GLOB_RECURSE KERNEL_ASM_SOURCES
    ${SRC}/arch/i686/*.asm
)
add_library(kernel OBJECT ${KERNEL_C_SOURCES} ${KERNEL_ASM_SOURCES})

# libk headers treated as system headers
target_include_directories(kernel SYSTEM PRIVATE ${SRC}/libk)
target_include_directories(kernel PRIVATE ${SRC})

# Target: sodium.bin
add_executable(sodium.bin
    $<TARGET_OBJECTS:bootloader>
    $<TARGET_OBJECTS:kernel>
)

execute_process(
    COMMAND i686-elf-gcc -print-libgcc-file-name
    OUTPUT_VARIABLE LIBGCC_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Using libgcc: ${LIBGCC_PATH}")

add_compile_options(-msse -msse2 -mfpmath=sse)
add_compile_options(-mno-mmx)

target_link_options(sodium.bin PRIVATE
    -nostartfiles
    -nodefaultlibs
    -Wl,-z,noexecstack
    -T ${SRC}/bootloader/i686/linker.ld
)
target_link_libraries(sodium.bin PRIVATE ${LIBGCC_PATH})

# Target: sodium.dbg
add_custom_command(TARGET sodium.bin POST_BUILD
    COMMAND i686-elf-objcopy --only-keep-debug sodium.bin ${CMAKE_BINARY_DIR}/sodium.dbg
    COMMAND i686-elf-objcopy --strip-debug sodium.bin
    COMMENT "--> Extracting debug symbols"
)

# Target: sodium.iso
add_custom_target(sodium.iso
    DEPENDS sodium.bin
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/bin
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/iso/boot/grub
    COMMAND cp sodium.bin ${CMAKE_BINARY_DIR}/iso/bin/kernel.bin
    COMMAND cp ${SRC}/bootloader/i686/grub/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/
    COMMAND mkdir -p ${DIST}
    COMMAND grub-mkrescue -o ${DIST}/Sodium.iso ${CMAKE_BINARY_DIR}/iso
    COMMAND cp ${CMAKE_BINARY_DIR}/sodium.dbg ${DIST}/sodium.dbg
    COMMAND grub-file --is-x86-multiboot2 ${CMAKE_BINARY_DIR}/iso/bin/kernel.bin
        || (echo "ERROR: Not Multiboot2 compliant" && exit 1)
    COMMENT "--> Building Sodium.iso"
)
