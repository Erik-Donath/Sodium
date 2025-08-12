#include "arch.h"

#include "cpu/fpu/fpu.h"
#include "cpu/gdt/gdt.h"
#include "cpu/idt/idt.h"
#include "interrupts/irq/irq.h"
#include "memory/heap/heap.h"

#include <kernel/core/terminal.h>
#include <kernel/libc/stdio.h>
#include <kernel/memory/memory.h>

#include <kernel/core/error.h>
#include <kernel/kernel.h>

static bool terminal_initialized = false;

static void ok(const char* msg) {
    printf(DEFAULT_COLOR "[ " SUCCESS_COLOR "OK" DEFAULT_COLOR " ] %s\n", msg);
}
/*
static void failed(const char* msg) {
    printf(DEFAULT_COLOR "[ " ERROR_COLOR "FAILED" DEFAULT_COLOR " ] %s\n", msg);
}*/

static void welcome() {
    puts("\033[0m" DEFAULT_COLOR "Welcome to " SODIUM_COLOR "Sodium" DEFAULT_COLOR "!\n");
}

static void irq_void(ISR_Registers*) {}

extern char mb_header_start;

void pre_main(mb_info_ptr mb) {
    // Setup Terminal
    terminal_init();
    welcome();
    ok("Terminal initialized");
    terminal_initialized = true;

    // Setup CPU Tables
    i686_FPU_Initialize();
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    ok("CPU Tables initialized");

    // Setup IRQ
    i686_IRQ_ERROR_t irq_err = i686_IRQ_Init();
    if(irq_err) PANIC("Failed to initialize IRQ");

    // Register Devices
    //i686_IRQ_RegisterHandler(INT_TIMER, timer);
    i686_IRQ_RegisterHandler(INT_TIMER, irq_void);
    i686_IRQ_RegisterHandler(INT_MOUSE, irq_void);
    i686_IRQ_RegisterHandler(INT_KEYBOARD, irq_void);
    ok("IRQ initialized");

    // Parse multiboot Information
    MB_ERROR_t mb_err = mb_parse(mb);
    if(mb_err) PANIC("Failed to load multiboot info");
    ok("Loaded multiboot info");

    // Print multiboot Information
    puts(INFO_COLOR);
    mb_print(mb);
    puts(DEFAULT_COLOR);

    // Init Heap
    MEM_ERROR_t mem_err = i686_memory_init();
    if(mem_err) PANIC("Failed to allocate heap block");
    ok("Allocated heap block");

    // Print Heap Information
    puts(INFO_COLOR);
    i686_memory_info();
    puts(DEFAULT_COLOR "\n");

    // Finish
    printf("System located at: %p\n", (void*)&mb_header_start);

    kmain();
    while(true) {}
}
