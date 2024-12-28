#pragma once

// ASN_CALL used when Assembly calls this funktion in C and C_CALL when C calls a funktion in Assembly
#define ASM_CALL __attribute__((cdecl))
#define C_CALL __attribute__((cdecl))

// Defined for structs that has to be as big as they realy are (No Padding). Mostly used when dealing with hardware interfaces.
#define PACKED __attribute__((packed))

// Used for Array Operations. (Can only be used on Arrays that are defined as a constant; no malloc)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAY_FIRST(x) (x[0])
#define ARRAY_LAST(x) (x[ARRAY_SIZE(x) - 1])