#include "arch.h"

#include "cpu/fpu/fpu.h"
#include "cpu/gdt/gdt.h"
#include "cpu/idt/idt.h"
#include "interrupts/irq/irq.h"
#include "memory/heap/heap.h"
#include "boot/multiboot2/multiboot.h"

#include "kernel/core/terminal.h"
#include "kernel/libc/stdio.h"
#include "kernel/memory/memory.h"

#include "kernel/core/error.h"
#include "kernel/kernel.h"

#define defaultColor Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define errorColor Color(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK)
#define successColor Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define sodiumColor Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)
#define infoColor Color(TERMINAL_COLOR_LIGHT_GREY, TERMINAL_COLOR_BLACK)

static bool terminal_initialized = false;

static void ok(const char* msg) {
    printf("%s[ %sOK%s ] %s\n", defaultColor, successColor, defaultColor, msg);
}
/*
static void failed(const char* msg) {
    printf("%s[ %sFAILED%s ] %s\n", defaultColor, errorColor, defaultColor, msg);
}*/

static void welcome() {
    puts("\033[0m" defaultColor "Welcome to " sodiumColor "Sodium" defaultColor "!\n");
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
    puts(infoColor);
    mb_print(mb);
    puts(defaultColor);

    // Init Heap
    MEM_ERROR_t mem_err = i686_memory_init();
    if(mem_err) PANIC("Failed to allocate heap block");
    ok("Allocated heap block");

    // Print Heap Information
    puts(infoColor);
    i686_memory_info();
    puts(defaultColor "\n");

    // Finish
    printf("System located at: %p\n", (void*)&mb_header_start);

    kmain();
    while(true) {}
}
