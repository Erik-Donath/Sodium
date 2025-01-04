#include <kernel/stdio.h>

#include "definition.h"
#include "multiboot.h"

bool mb_parse(mb_info_ptr mb) {
    const mb2_header* header = (const mb2_header*)mb;
    if(!header || header->reserved) {
        return false;
    }
    return true;
}
