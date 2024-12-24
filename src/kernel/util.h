#pragma once

// ASN_CALL used when Assembly calls this funktion in C and C_CALL when C calls a funktion in Assembly
#define ASM_CALL __attribute__((cdecl))
#define C_CALL __attribute__((cdecl))

// Defined for structs that has to be as big as they realy are (No Padding). Mostly used when dealing with hardware interfaces.
#define PACKED __attribute__((packed))