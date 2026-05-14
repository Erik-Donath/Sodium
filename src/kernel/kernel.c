#include "kernel.h"

#include <stdio.h>

void k_main(void) {
    printf("Kernel loaded\n");

    while(1) ;
    return;
}