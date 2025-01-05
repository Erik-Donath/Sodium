#include "arch.h"

#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "idt/idt.h"

#include <kernel/terminal.h>
#include <kernel/stdio.h>

#define dc Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define ec Color(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK)
#define gc Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define cc Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)

static void ok(const char* msg) {
    printf("%s[ %sOK%s ] %s\n", dc, gc, dc, msg);
}

static void failed(const char* msg) {
    printf("%s[ %sFAILED%s ] %s\n", dc, ec, dc, msg);
}

static void welcome() {
    puts("\033[0m" dc "Welcome to " cc "Sodium" dc "!\n");
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
    // Parse multiboot Information
    bool success = mb_parse(mb);
    if(!success) {
        failed("Failed to load multiboot info");
        return;
    }
    ok("Loaded multiboot info");
    terminal_putc('\n');
    const memory_info* mem_info = mb_getMemoryInfo();

    // Print Memory Info
    printf("Memory Info:\n\tmem_lower = %u\n\tmem_upper = %u\n\tMemory Map:\n", mem_info->mem_lower, mem_info->mem_upper);
    for(uint32_t i = 0; i < mem_info->entry_count; i++) {
        memory_map_entry mem_entry = mem_info->entries[i];
        printf("\t\tTYPE = %u", mem_entry.type);
        puts(", BASE = 0x");
        print_hex64(mem_entry.base_addr, false);
        puts(", LENGHT = ");
        print_hex64(mem_entry.lenght, false);
        putc('\n');
    }
    //mb_print(mb);

    // Color test
    putc('\n');
    terminal_testColor();
    puts("\nDONE!\n");

    while(true) {}
}
