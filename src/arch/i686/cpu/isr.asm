bits 32

section .text

%include "arch/i686/cpu/segments.inc"
extern i686_isr_handlers

%macro ISR_COMMON 1
    ; Push all Registers to Stack
    pusha
    push ds
    push es
    push fs
    push gs
 
    ; Load kernel data segments
    mov ax, i686_KERNEL_DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Push esp to stack which will point at the first parameter that has been pushed by the cpu. This structure should be the exact same as i686_isr_cpu_state in C.
    push esp
    call [i686_isr_handlers + %1 * 4]
    add esp, 4
 
    pop gs
    pop fs
    pop es
    pop ds
    popa

    ; discard int_num + error_code
    add esp, 8
    iret
%endmacro

; Some interrupts dont provide an error code. Therefore we first need to push an dummy error code on the stack to not curropt the cpu_state struct.
%macro ISR_NOERR 1
global i686_isr_%1
i686_isr_%1:
    push dword 0
    push dword %1
    ISR_COMMON %1
%endmacro

%macro ISR_ERR 1
global i686_isr_%1
i686_isr_%1:
    push dword %1
    ISR_COMMON %1
%endmacro

; The first 32 interrupts are exeptions
ISR_NOERR  0    ; #DE  Divide Error
ISR_NOERR  1    ; #DB  Debug
ISR_NOERR  2    ;      NMI
ISR_NOERR  3    ; #BP  Breakpoint
ISR_NOERR  4    ; #OF  Overflow
ISR_NOERR  5    ; #BR  Bound Range Exceeded
ISR_NOERR  6    ; #UD  Invalid Opcode
ISR_NOERR  7    ; #NM  Device Not Available
ISR_ERR    8    ; #DF  Double Fault
ISR_NOERR  9    ;      Coprocessor Segment Overrun (legacy)
ISR_ERR   10    ; #TS  Invalid TSS
ISR_ERR   11    ; #NP  Segment Not Present
ISR_ERR   12    ; #SS  Stack-Segment Fault
ISR_ERR   13    ; #GP  General Protection Fault
ISR_ERR   14    ; #PF  Page Fault
ISR_NOERR 15    ;      Reserved
ISR_NOERR 16    ; #MF  x87 FPU Exception
ISR_ERR   17    ; #AC  Alignment Check
ISR_NOERR 18    ; #MC  Machine Check
ISR_NOERR 19    ; #XM  SIMD FPU Exception
ISR_NOERR 20    ; #VE  Virtualisation Exception
ISR_ERR   21    ; #CP  Control Protection Exception
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR   29    ; #HV  Hypervisor Injection (AMD)
ISR_ERR   30    ; #VC  VMM Communication (AMD)
ISR_NOERR 31    ; #SX  Security Exception

; All other 32 - 255 interrupts are user defined
%assign i 32
%rep (256-32)
ISR_NOERR i
%assign i i+1
%endrep

section .rodata
global i686_isr_stub
i686_isr_stub:
%assign i 0
%rep 256
    dd i686_isr_%+i
%assign i i+1
%endrep
