; multiboot2.inc

; GNU Multiboot2 Specification:
; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
; Multiboot2 Header Specification:
; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Header-layout

; Magic number and architecture
MB2_MAGIC                equ 0xE85250D6
MB2_BOOTLOADER_MAGIC     equ 0x36d76289
MB2_ARCH_I386            equ 0
MB2_ARCH_MIPS32          equ 4

; Header tags
MB2_HEADER_TAG_END               equ 0
MB2_HEADER_TAG_INFORMATION_REQUEST equ 1
MB2_HEADER_TAG_ADDRESS           equ 2
MB2_HEADER_TAG_ENTRY_ADDRESS     equ 3
MB2_HEADER_TAG_CONSOLE_FLAGS     equ 4
MB2_HEADER_TAG_FRAMEBUFFER       equ 5
MB2_HEADER_TAG_MODULE_ALIGN      equ 6
MB2_HEADER_TAG_EFI_BS            equ 7
MB2_HEADER_TAG_ENTRY_ADDRESS_EFI32 equ 8
MB2_HEADER_TAG_ENTRY_ADDRESS_EFI64 equ 9
MB2_HEADER_TAG_RELOCATABLE       equ 10

; Information request flags
MB2_INFO_REQUEST_MEMORY          equ 0
MB2_INFO_REQUEST_BOOTDEV         equ 1
MB2_INFO_REQUEST_CMDLINE         equ 2
MB2_INFO_REQUEST_MODULES         equ 3
MB2_INFO_REQUEST_MEMORY_MAP      equ 6
MB2_INFO_REQUEST_VBE             equ 7
MB2_INFO_REQUEST_FRAMEBUFFER     equ 8
MB2_INFO_REQUEST_ELF_SECTIONS    equ 9
MB2_INFO_REQUEST_APM             equ 10
MB2_INFO_REQUEST_EFI32           equ 11
MB2_INFO_REQUEST_EFI64           equ 12
MB2_INFO_REQUEST_SMBIOS          equ 13
MB2_INFO_REQUEST_ACPI_OLD        equ 14
MB2_INFO_REQUEST_ACPI_NEW        equ 15
MB2_INFO_REQUEST_NETWORK         equ 16
MB2_INFO_REQUEST_EFI_MMAP        equ 17
MB2_INFO_REQUEST_EFI_BS          equ 18
MB2_INFO_REQUEST_EFI32_IH        equ 19
MB2_INFO_REQUEST_EFI64_IH        equ 20
MB2_INFO_REQUEST_LOAD_BASE_ADDR  equ 21

; Other constants
MB2_CONSOLE_FLAGS_CONSOLE_REQUIRED equ 1
MB2_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED equ 2
MB2_FRAMEBUFFER_TYPE_INDEXED     equ 0
MB2_FRAMEBUFFER_TYPE_RGB         equ 1
MB2_FRAMEBUFFER_TYPE_EGA_TEXT    equ 2
MB2_RELOCATABLE_PREFER_LOW       equ 0
MB2_RELOCATABLE_PREFER_HIGH      equ 1
MB2_RELOCATABLE_PREFER_NONE      equ 2

; Base macro for tags
%macro MB2_TAG 3
    dw %1  ; type
    dw %2  ; flags
    dd %3  ; size
%endmacro

; Padding macro
%macro MB2_PADDING 0
    %assign padding (8 - ($ - $$) % 8) % 8
    %if padding > 0
        times padding db 0
    %endif
%endmacro

; Macros for specific tags
%macro MB2_HEADER_START 2
    dd MB2_MAGIC
    dd %1
    dd %2
    dd -(MB2_MAGIC + %1 + %2)
%endmacro

%macro MB2_END_TAG 0
    MB2_TAG MB2_HEADER_TAG_END, 0, 8
    MB2_PADDING
%endmacro

%macro MB2_INFORMATION_REQUEST_TAG 1-*
    MB2_TAG MB2_HEADER_TAG_INFORMATION_REQUEST, 0, 8 + (%0 * 4)
    %rep %0
        dd %1
        %rotate 1
    %endrep
    MB2_PADDING
%endmacro

%macro MB2_ADDRESS_TAG 4
    MB2_TAG MB2_HEADER_TAG_ADDRESS, 0, 24
    dd %1  ; header_addr
    dd %2  ; load_addr
    dd %3  ; load_end_addr
    dd %4  ; bss_end_addr
    MB2_PADDING
%endmacro

%macro MB2_ENTRY_ADDRESS_TAG 1
    MB2_TAG MB2_HEADER_TAG_ENTRY_ADDRESS, 0, 12
    dd %1  ; entry_addr
    MB2_PADDING
%endmacro

%macro MB2_CONSOLE_FLAGS_TAG 1
    MB2_TAG MB2_HEADER_TAG_CONSOLE_FLAGS, 0, 12
    dd %1  ; console_flags
    MB2_PADDING
%endmacro

%macro MB2_FRAMEBUFFER_TAG 5
    MB2_TAG MB2_HEADER_TAG_FRAMEBUFFER, 0, 20
    dd %1  ; width
    dd %2  ; height
    dd %3  ; depth
    dd %4  ; fb_type
    dd %5  ; fb_flags
    MB2_PADDING
%endmacro

%macro MB2_MODULE_ALIGN_TAG 0
    MB2_TAG MB2_HEADER_TAG_MODULE_ALIGN, 0, 8
    MB2_PADDING
%endmacro

%macro MB2_EFI_BS_TAG 0
    MB2_TAG MB2_HEADER_TAG_EFI_BS, 0, 8
    MB2_PADDING
%endmacro

%macro MB2_ENTRY_ADDRESS_EFI32_TAG 1
    MB2_TAG MB2_HEADER_TAG_ENTRY_ADDRESS_EFI32, 0, 12
    dd %1  ; entry_addr
    MB2_PADDING
%endmacro

%macro MB2_ENTRY_ADDRESS_EFI64_TAG 1
    MB2_TAG MB2_HEADER_TAG_ENTRY_ADDRESS_EFI64, 0, 12
    dd %1  ; entry_addr
    MB2_PADDING
%endmacro

%macro MB2_RELOCATABLE_TAG 3
    MB2_TAG MB2_HEADER_TAG_RELOCATABLE, 0, 24
    dd %1  ; min_addr
    dd %2  ; max_addr
    dd %3  ; align
    MB2_PADDING
%endmacro
