#pragma once

void INFO_ParseMultiboot(void* mb_info);
void* INFO_GetMultiboot();

// More funktions

/*
    Information about the multiboot info struct:
    http://www.brokenthorn.com/Resources/OSDevMulti.html
    --> https://www.gnu.org/software/grub/manual/multiboot2/html_node/Boot-information-format.html
    https://git.savannah.gnu.org/cgit/grub.git/tree/doc/multiboot2.h?h=multiboot2  
*/
