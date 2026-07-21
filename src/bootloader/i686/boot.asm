bits 32

%include "arch/i686/mem/vmm.inc"

; Boot code
section .boot
extern i686_pre_kernel
extern bss_start
extern bss_end
extern stack_top
global _start

%define MB2_MAGIC               0x36D76289

%define PAGE_DIRECTORY_ENTRIES  1024
%define KERNEL_PDE_INDEX        (KERNEL_VIRTUAL_BASE >> 22) ; Index of the kernel's PDE in the page directory

_start:
    cli ; Clear interrupts

    ; Setup the stack temporarily
    mov esp, (stack_top - KERNEL_VIRTUAL_BASE)
    mov ebp, esp

    ; Verify Multiboot2 magic
    cmp eax, MB2_MAGIC
    jne .invalid_magic
    
    ; save multiboot info pointer
    push ebx

    ; Check CPUID is available (bit 21 of EFLAGS can be toggled)
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 0x200000
    push eax
    popfd
    pushfd
    pop eax
    cmp eax, ecx
    je .no_cpuid            ; CPUID not supported at all

    ; Check PSE support (CPUID.1:EDX bit 3)
    mov eax, 1
    cpuid
    test edx, (1 << 3)
    jz .no_pse

    ; restore multiboot info pointer
    pop ebx

    ; set up the page directory for the kernel
    mov eax, bootstrap_page_dir
    mov cr3, eax

    ; Enable PSE (Page Size Extension)
    mov eax, cr4
    or eax, (1 << 4)
    mov cr4, eax

    ; Enable paging
    mov eax, cr0
    or eax, (1 << 31)
    mov cr0, eax

    jmp _higher_half_entry

.invalid_magic:
.no_cpuid:
.no_pse:
    cli
    hlt
    jmp .invalid_magic

%define PDE_PRESENT        (1 << 0)
%define PDE_READ_WRITE     (1 << 1)
%define PDE_PAGE_SIZE_4MB  (1 << 7)

; @brief Emit one 4MB page directory entry.
; %1 = physical base address (must be 4MB-aligned)
%macro PDE_4MB 1
    dd (%1) | PDE_PRESENT | PDE_READ_WRITE | PDE_PAGE_SIZE_4MB
%endmacro

align 4096
bootstrap_page_dir:
    PDE_4MB 0x00000000                  ; PDE[0]: identity mapping for the first 4MB of physical memory
    times (KERNEL_PDE_INDEX - 1) dd 0   ; Fill the rest of the PDEs with zeros until the kernel's PDE index
    
    ; Fill the kernel's PDE with a 4MB page mapping to the kernel's physical base address
    %assign i 0
    %rep KERNEL_MIRROR_PAGES
        PDE_4MB (i * KERNEL_PDE_FRAME_SIZE)
        %assign i i + 1
    %endrep

    times (PAGE_DIRECTORY_ENTRIES - KERNEL_PDE_INDEX - KERNEL_MIRROR_PAGES) dd 0 ; Fill the rest of the PDEs with zeros
end_bootstrap_page_dir:

section .text
_higher_half_entry:
    ; Set up 16KB stack
    mov esp, stack_top
    mov ebp, esp

    ; Zero BSS
    mov edi, bss_start
    mov ecx, bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; Call pre_kernel(mb_info*)
    add ebx, KERNEL_VIRTUAL_BASE ; Adjust the multiboot info pointer to the higher half
    push ebx
    call i686_pre_kernel

.shutdown:
    ; QEMU spesific shutdown sequence
    mov dx, 0x604
    mov ax, 0x2000
    out dx, ax

    ; QEMU (older) / Bochs shutdown sequence
    mov dx, 0xB004
    mov ax, 0x2000
    out dx, ax

    ; VirtualBox
    mov dx, 0x4004
    mov ax, 0x3400
    out dx, ax

    ; FIXME: Call i686_acpi_shutdown() here once we implement ACPI support.

    ; Halt the system
    cli
    hlt
    jmp .shutdown
