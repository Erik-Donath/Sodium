#pragma once

#define i686_VGA_TEXT_BASE    0xB8000
#define i686_VGA_GRAPH_BASE   0xA0000

#define i686_VGA_PORT_MISC_WRITE  0x3C2
#define i686_VGA_PORT_MISC_READ   0x3CC
#define i686_VGA_PORT_SEQ_INDEX   0x3C4
#define i686_VGA_PORT_SEQ_DATA    0x3C5
#define i686_VGA_PORT_GC_INDEX    0x3CE
#define i686_VGA_PORT_GC_DATA     0x3CF
#define i686_VGA_PORT_CRTC_INDEX  0x3D4
#define i686_VGA_PORT_CRTC_DATA   0x3D5
#define i686_VGA_PORT_AC_INDEX    0x3C0   // write index AND data (flip-flop)
#define i686_VGA_PORT_AC_READ     0x3C1
#define i686_VGA_PORT_INSTAT1     0x3DA   // read resets AC flip-flop
