#pragma once

void SYSINFO_parse_multiboot(void* mb_info);
void SYSINFO_print_multiboot_info();

/*
    Information about the multiboot info struct:
    http://www.brokenthorn.com/Resources/OSDevMulti.html
    --> https://www.gnu.org/software/grub/manual/multiboot2/html_node/Boot-information-format.html
    https://git.savannah.gnu.org/cgit/grub.git/tree/doc/multiboot2.h?h=multiboot2  
*/