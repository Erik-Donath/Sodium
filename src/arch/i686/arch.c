
#include "arch.h"
#include "cpu/fpu/fpu.h"
#include "cpu/gdt/gdt.h"
#include "cpu/idt/idt.h"
#include "interrupts/irq/irq.h"
#include "memory/heap/heap.h"
#include "drivers/vga/vga.h"
#include "drivers/debug/debug.h"
#include <kernel/core/terminal.h>
#include <kernel/libc/stdio.h>
#include <kernel/memory/memory.h>
#include <kernel/core/error.h>
#include <kernel/kernel.h>


// Indicates if the terminal has been initialized
static bool terminal_initialized = false;


// Print a success message to the terminal
static void ok(const char* msg) {
    printf(DEFAULT_COLOR "[ " SUCCESS_COLOR "OK" DEFAULT_COLOR " ] %s\n", msg);
}


// Print welcome message
static void welcome() {
    puts("\033[0m" DEFAULT_COLOR "Welcome to " SODIUM_COLOR "Sodium" DEFAULT_COLOR "!\n");
}


// Default IRQ handler (does nothing)
static void irq_void(ISR_Registers*) {}


// List of available terminal display drivers
static const display_driver_t* terminal_drivers[] = {&vga_driver, &debug_driver};


extern char mb_header_start;


// Main entry point before kernel main
void pre_main(mb_info_ptr mb) {
    // 1. Parse multiboot information
    MB_ERROR_t mb_err = mb_parse(mb);
    if (mb_err) PANIC("Failed to load multiboot info");
    ok("Loaded multiboot info");

    // 2. Initialize heap/memory management
    MEM_ERROR_t mem_err = i686_memory_init();
    if (mem_err) PANIC("Failed to allocate heap block");
    ok("Allocated heap block");

    // 3. Initialize terminal and display drivers (after heap!)
    terminal_init(terminal_drivers, 2);
    welcome();
    ok("Terminal initialized");
    terminal_initialized = true;

    // 4. Set up CPU tables (FPU, GDT, IDT)
    i686_FPU_Initialize();
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    ok("CPU tables initialized");

    // 5. Initialize IRQs and register handlers
    i686_IRQ_ERROR_t irq_err = i686_IRQ_Init();
    if (irq_err) PANIC("Failed to initialize IRQ");
    i686_IRQ_RegisterHandler(INT_TIMER, irq_void);
    i686_IRQ_RegisterHandler(INT_MOUSE, irq_void);
    i686_IRQ_RegisterHandler(INT_KEYBOARD, irq_void);
    ok("IRQ initialized");

    // Print multiboot information
    puts(INFO_COLOR);
    mb_print(mb);
    puts(DEFAULT_COLOR);

    // Print heap information
    puts(INFO_COLOR);
    i686_memory_info();
    puts(DEFAULT_COLOR "\n");

    // 6. Start kernel
    printf("System located at: %p\n", (void*)&mb_header_start);
    kmain();
    while (true) {}
}
