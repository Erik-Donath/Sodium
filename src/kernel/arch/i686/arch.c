#include "arch.h"

#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "irq/irq.h"

#include <kernel/terminal.h>
#include <kernel/stdio.h>

#define defaultColor Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define errorColor Color(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK)
#define successColor Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define sodiumColor Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)
#define infoColor Color(TERMINAL_COLOR_LIGHT_GREY, TERMINAL_COLOR_BLACK)

static void ok(const char* msg) {
    printf("%s[ %sOK%s ] %s\n", defaultColor, successColor, defaultColor, msg);
}

static void failed(const char* msg) {
    printf("%s[ %sFAILED%s ] %s\n", defaultColor, errorColor, defaultColor, msg);
}

static void welcome() {
    puts("\033[0m" defaultColor "Welcome to " sodiumColor "Sodium" defaultColor "!\n");
}

static void timer(ISR_Registers*) {
    static uint32_t count = 0;
    printf("\rTimer: %d", ++count);
}

void pre_main(mb_info_ptr mb) {
    // Setup CPU
    i686_FPU_Initialize();

    // Setup Terminal
    terminal_init();
    welcome();
    ok("Terminal initialized");

    // Setup CPU Tables
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    ok("CPU Tables initialized");

    // Setup IRQ
    {
        bool success = i686_IRQ_Init();
        if(!success) {
            failed("Failed to initialize IRQ");
            return;
        }

        i686_IRQ_RegisterHandler(INT_TIMER, timer);
        ok("IRQ initialized");
    }

    // Parse multiboot Information
    {
        bool success = mb_parse(mb);
        if(!success) {
            failed("Failed to load multiboot info");
            return;
        }

        ok("Loaded multiboot info");
        puts(infoColor);
        mb_print(mb);
        puts(defaultColor);
    }

    /*
    // Print Memory Info
    const memory_info* mem_info = mb_getMemoryInfo();
    printf("\nMemory Info:\n\tmem_lower = %u\n\tmem_upper = %u\n\tMemory Map:\n", mem_info->mem_lower, mem_info->mem_upper);
    for(uint32_t i = 0; i < mem_info->entry_count; i++) {
        memory_map_entry mem_entry = mem_info->entries[i];
        printf("\t\tTYPE = %u", mem_entry.type);
        puts(", BASE = 0x");
        print_hex64(mem_entry.base_addr, false);
        puts(", LENGHT = ");
        print_hex64(mem_entry.lenght, false);
        putc('\n');
    }
    */

    // Color test
    puts("\n Stated \n\n");
    terminal_testColor();

    puts("\n\033[0m" defaultColor "> ");
    putc('\n');

    while(true) {}
}
