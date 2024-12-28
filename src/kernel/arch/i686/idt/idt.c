#include <stdint.h>
#include <kernel/util.h>
#include "isr.h"
#include "gdt.h"
#include "idt.h"

/*
| Bits    | Description                             |
|---------|-----------------------------------------|
| 0-15    | Offset (lower 16 bits)                  |
| 16-31   | Selector (segment selector)             |
| 32-39   | Reserved (must be zero)                 |
| 40-43   | Type (gate type: task, interrupt, trap) |
| 44      | Storage Segment (always 0 for IDT)      |
| 45-46   | Descriptor Privilege Level (DPL)        |
| 47      | Present flag (P)                        |
| 48-63   | Offset (higher 16 bits)                 |
*/

typedef struct {
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t base_high;
} PACKED idt_entry;

typedef struct {
    uint16_t limit;
    idt_entry* base;
} PACKED idt_pointer;

typedef enum {
    // Gate Types
    IDT_FLAG_GATE_TASK              = 0x5,  // Task gate
    IDT_FLAG_GATE_16BIT_INT         = 0x6,  // 16-bit interrupt gate
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,  // 16-bit trap gate
    IDT_FLAG_GATE_32BIT_INT         = 0xE,  // 32-bit interrupt gate
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,  // 32-bit trap gate

    // Descriptor Privilege Levels (DPL)
    IDT_FLAG_RING0                  = 0x00,  // Ring 0 (kernel)
    IDT_FLAG_RING1                  = 0x20,  // Ring 1
    IDT_FLAG_RING2                  = 0x40,  // Ring 2
    IDT_FLAG_RING3                  = 0x60,  // Ring 3 (user)

    // Present Bit
    IDT_FLAG_PRESENT                = 0x80   // Segment present
} IDT_FLAGS;

#define IDT_ENTRY(isr, selector, flags) { \
    (uint16_t)((uint32_t)(isr) & 0xFFFF), \
    (selector), \
    (0), \
    (flags), \
    (uint16_t)(((uint32_t)(isr) >> 16) & 0xFFFF) \
}

extern void isr0();

static idt_entry idt[256];
static idt_pointer idt_ptr = {
    .limit = sizeof(idt) - 1,
    .base = idt
};

static inline void i686_IDT_SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num] = (idt_entry){
        .base_low = base & 0xFFFF,
        .segment_selector = selector,
        .reserved = 0,
        .flags = flags,
        .base_high = (base >> 16) & 0xFFFF
    };
}

void C_CALL i686_IDT_Flush(idt_pointer* ptr);
void i686_IDT_Initialize() {
    i686_ISR_Initialize();
    i686_IDT_Flush(&idt_ptr);
}

void i686_ISR_Initialize() {
    #define DEFAULT GDT_KERNEL_CODE_SEGMENT, (IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT)
    i686_IDT_SetGate(0, (uint32_t)i686_ISR_0, DEFAULT);
    i686_IDT_SetGate(1, (uint32_t)i686_ISR_1, DEFAULT);
    i686_IDT_SetGate(2, (uint32_t)i686_ISR_2, DEFAULT);
    i686_IDT_SetGate(3, (uint32_t)i686_ISR_3, DEFAULT);
    i686_IDT_SetGate(4, (uint32_t)i686_ISR_4, DEFAULT);
    i686_IDT_SetGate(5, (uint32_t)i686_ISR_5, DEFAULT);
    i686_IDT_SetGate(6, (uint32_t)i686_ISR_6, DEFAULT);
    i686_IDT_SetGate(7, (uint32_t)i686_ISR_7, DEFAULT);
    i686_IDT_SetGate(8, (uint32_t)i686_ISR_8, DEFAULT);
    i686_IDT_SetGate(9, (uint32_t)i686_ISR_9, DEFAULT);
    i686_IDT_SetGate(10, (uint32_t)i686_ISR_10, DEFAULT);
    i686_IDT_SetGate(11, (uint32_t)i686_ISR_11, DEFAULT);
    i686_IDT_SetGate(12, (uint32_t)i686_ISR_12, DEFAULT);
    i686_IDT_SetGate(13, (uint32_t)i686_ISR_13, DEFAULT);
    i686_IDT_SetGate(14, (uint32_t)i686_ISR_14, DEFAULT);
    i686_IDT_SetGate(15, (uint32_t)i686_ISR_15, DEFAULT);
    i686_IDT_SetGate(16, (uint32_t)i686_ISR_16, DEFAULT);
    i686_IDT_SetGate(17, (uint32_t)i686_ISR_17, DEFAULT);
    i686_IDT_SetGate(18, (uint32_t)i686_ISR_18, DEFAULT);
    i686_IDT_SetGate(19, (uint32_t)i686_ISR_19, DEFAULT);
    i686_IDT_SetGate(20, (uint32_t)i686_ISR_20, DEFAULT);
    i686_IDT_SetGate(21, (uint32_t)i686_ISR_21, DEFAULT);
    i686_IDT_SetGate(22, (uint32_t)i686_ISR_22, DEFAULT);
    i686_IDT_SetGate(23, (uint32_t)i686_ISR_23, DEFAULT);
    i686_IDT_SetGate(24, (uint32_t)i686_ISR_24, DEFAULT);
    i686_IDT_SetGate(25, (uint32_t)i686_ISR_25, DEFAULT);
    i686_IDT_SetGate(26, (uint32_t)i686_ISR_26, DEFAULT);
    i686_IDT_SetGate(27, (uint32_t)i686_ISR_27, DEFAULT);
    i686_IDT_SetGate(28, (uint32_t)i686_ISR_28, DEFAULT);
    i686_IDT_SetGate(29, (uint32_t)i686_ISR_29, DEFAULT);
    i686_IDT_SetGate(30, (uint32_t)i686_ISR_30, DEFAULT);
    i686_IDT_SetGate(31, (uint32_t)i686_ISR_31, DEFAULT);
    i686_IDT_SetGate(32, (uint32_t)i686_ISR_32, DEFAULT);
    i686_IDT_SetGate(33, (uint32_t)i686_ISR_33, DEFAULT);
    i686_IDT_SetGate(34, (uint32_t)i686_ISR_34, DEFAULT);
    i686_IDT_SetGate(35, (uint32_t)i686_ISR_35, DEFAULT);
    i686_IDT_SetGate(36, (uint32_t)i686_ISR_36, DEFAULT);
    i686_IDT_SetGate(37, (uint32_t)i686_ISR_37, DEFAULT);
    i686_IDT_SetGate(38, (uint32_t)i686_ISR_38, DEFAULT);
    i686_IDT_SetGate(39, (uint32_t)i686_ISR_39, DEFAULT);
    i686_IDT_SetGate(40, (uint32_t)i686_ISR_40, DEFAULT);
    i686_IDT_SetGate(41, (uint32_t)i686_ISR_41, DEFAULT);
    i686_IDT_SetGate(42, (uint32_t)i686_ISR_42, DEFAULT);
    i686_IDT_SetGate(43, (uint32_t)i686_ISR_43, DEFAULT);
    i686_IDT_SetGate(44, (uint32_t)i686_ISR_44, DEFAULT);
    i686_IDT_SetGate(45, (uint32_t)i686_ISR_45, DEFAULT);
    i686_IDT_SetGate(46, (uint32_t)i686_ISR_46, DEFAULT);
    i686_IDT_SetGate(47, (uint32_t)i686_ISR_47, DEFAULT);
    i686_IDT_SetGate(48, (uint32_t)i686_ISR_48, DEFAULT);
    i686_IDT_SetGate(49, (uint32_t)i686_ISR_49, DEFAULT);
    i686_IDT_SetGate(50, (uint32_t)i686_ISR_50, DEFAULT);
    i686_IDT_SetGate(51, (uint32_t)i686_ISR_51, DEFAULT);
    i686_IDT_SetGate(52, (uint32_t)i686_ISR_52, DEFAULT);
    i686_IDT_SetGate(53, (uint32_t)i686_ISR_53, DEFAULT);
    i686_IDT_SetGate(54, (uint32_t)i686_ISR_54, DEFAULT);
    i686_IDT_SetGate(55, (uint32_t)i686_ISR_55, DEFAULT);
    i686_IDT_SetGate(56, (uint32_t)i686_ISR_56, DEFAULT);
    i686_IDT_SetGate(57, (uint32_t)i686_ISR_57, DEFAULT);
    i686_IDT_SetGate(58, (uint32_t)i686_ISR_58, DEFAULT);
    i686_IDT_SetGate(59, (uint32_t)i686_ISR_59, DEFAULT);
    i686_IDT_SetGate(60, (uint32_t)i686_ISR_60, DEFAULT);
    i686_IDT_SetGate(61, (uint32_t)i686_ISR_61, DEFAULT);
    i686_IDT_SetGate(62, (uint32_t)i686_ISR_62, DEFAULT);
    i686_IDT_SetGate(63, (uint32_t)i686_ISR_63, DEFAULT);
    i686_IDT_SetGate(64, (uint32_t)i686_ISR_64, DEFAULT);
    i686_IDT_SetGate(65, (uint32_t)i686_ISR_65, DEFAULT);
    i686_IDT_SetGate(66, (uint32_t)i686_ISR_66, DEFAULT);
    i686_IDT_SetGate(67, (uint32_t)i686_ISR_67, DEFAULT);
    i686_IDT_SetGate(68, (uint32_t)i686_ISR_68, DEFAULT);
    i686_IDT_SetGate(69, (uint32_t)i686_ISR_69, DEFAULT);
    i686_IDT_SetGate(70, (uint32_t)i686_ISR_70, DEFAULT);
    i686_IDT_SetGate(71, (uint32_t)i686_ISR_71, DEFAULT);
    i686_IDT_SetGate(72, (uint32_t)i686_ISR_72, DEFAULT);
    i686_IDT_SetGate(73, (uint32_t)i686_ISR_73, DEFAULT);
    i686_IDT_SetGate(74, (uint32_t)i686_ISR_74, DEFAULT);
    i686_IDT_SetGate(75, (uint32_t)i686_ISR_75, DEFAULT);
    i686_IDT_SetGate(76, (uint32_t)i686_ISR_76, DEFAULT);
    i686_IDT_SetGate(77, (uint32_t)i686_ISR_77, DEFAULT);
    i686_IDT_SetGate(78, (uint32_t)i686_ISR_78, DEFAULT);
    i686_IDT_SetGate(79, (uint32_t)i686_ISR_79, DEFAULT);
    i686_IDT_SetGate(80, (uint32_t)i686_ISR_80, DEFAULT);
    i686_IDT_SetGate(81, (uint32_t)i686_ISR_81, DEFAULT);
    i686_IDT_SetGate(82, (uint32_t)i686_ISR_82, DEFAULT);
    i686_IDT_SetGate(83, (uint32_t)i686_ISR_83, DEFAULT);
    i686_IDT_SetGate(84, (uint32_t)i686_ISR_84, DEFAULT);
    i686_IDT_SetGate(85, (uint32_t)i686_ISR_85, DEFAULT);
    i686_IDT_SetGate(86, (uint32_t)i686_ISR_86, DEFAULT);
    i686_IDT_SetGate(87, (uint32_t)i686_ISR_87, DEFAULT);
    i686_IDT_SetGate(88, (uint32_t)i686_ISR_88, DEFAULT);
    i686_IDT_SetGate(89, (uint32_t)i686_ISR_89, DEFAULT);
    i686_IDT_SetGate(90, (uint32_t)i686_ISR_90, DEFAULT);
    i686_IDT_SetGate(91, (uint32_t)i686_ISR_91, DEFAULT);
    i686_IDT_SetGate(92, (uint32_t)i686_ISR_92, DEFAULT);
    i686_IDT_SetGate(93, (uint32_t)i686_ISR_93, DEFAULT);
    i686_IDT_SetGate(94, (uint32_t)i686_ISR_94, DEFAULT);
    i686_IDT_SetGate(95, (uint32_t)i686_ISR_95, DEFAULT);
    i686_IDT_SetGate(96, (uint32_t)i686_ISR_96, DEFAULT);
    i686_IDT_SetGate(97, (uint32_t)i686_ISR_97, DEFAULT);
    i686_IDT_SetGate(98, (uint32_t)i686_ISR_98, DEFAULT);
    i686_IDT_SetGate(99, (uint32_t)i686_ISR_99, DEFAULT);
    i686_IDT_SetGate(100, (uint32_t)i686_ISR_100, DEFAULT);
    i686_IDT_SetGate(101, (uint32_t)i686_ISR_101, DEFAULT);
    i686_IDT_SetGate(102, (uint32_t)i686_ISR_102, DEFAULT);
    i686_IDT_SetGate(103, (uint32_t)i686_ISR_103, DEFAULT);
    i686_IDT_SetGate(104, (uint32_t)i686_ISR_104, DEFAULT);
    i686_IDT_SetGate(105, (uint32_t)i686_ISR_105, DEFAULT);
    i686_IDT_SetGate(106, (uint32_t)i686_ISR_106, DEFAULT);
    i686_IDT_SetGate(107, (uint32_t)i686_ISR_107, DEFAULT);
    i686_IDT_SetGate(108, (uint32_t)i686_ISR_108, DEFAULT);
    i686_IDT_SetGate(109, (uint32_t)i686_ISR_109, DEFAULT);
    i686_IDT_SetGate(110, (uint32_t)i686_ISR_110, DEFAULT);
    i686_IDT_SetGate(111, (uint32_t)i686_ISR_111, DEFAULT);
    i686_IDT_SetGate(112, (uint32_t)i686_ISR_112, DEFAULT);
    i686_IDT_SetGate(113, (uint32_t)i686_ISR_113, DEFAULT);
    i686_IDT_SetGate(114, (uint32_t)i686_ISR_114, DEFAULT);
    i686_IDT_SetGate(115, (uint32_t)i686_ISR_115, DEFAULT);
    i686_IDT_SetGate(116, (uint32_t)i686_ISR_116, DEFAULT);
    i686_IDT_SetGate(117, (uint32_t)i686_ISR_117, DEFAULT);
    i686_IDT_SetGate(118, (uint32_t)i686_ISR_118, DEFAULT);
    i686_IDT_SetGate(119, (uint32_t)i686_ISR_119, DEFAULT);
    i686_IDT_SetGate(120, (uint32_t)i686_ISR_120, DEFAULT);
    i686_IDT_SetGate(121, (uint32_t)i686_ISR_121, DEFAULT);
    i686_IDT_SetGate(122, (uint32_t)i686_ISR_122, DEFAULT);
    i686_IDT_SetGate(123, (uint32_t)i686_ISR_123, DEFAULT);
    i686_IDT_SetGate(124, (uint32_t)i686_ISR_124, DEFAULT);
    i686_IDT_SetGate(125, (uint32_t)i686_ISR_125, DEFAULT);
    i686_IDT_SetGate(126, (uint32_t)i686_ISR_126, DEFAULT);
    i686_IDT_SetGate(127, (uint32_t)i686_ISR_127, DEFAULT);
    i686_IDT_SetGate(128, (uint32_t)i686_ISR_128, DEFAULT);
    i686_IDT_SetGate(129, (uint32_t)i686_ISR_129, DEFAULT);
    i686_IDT_SetGate(130, (uint32_t)i686_ISR_130, DEFAULT);
    i686_IDT_SetGate(131, (uint32_t)i686_ISR_131, DEFAULT);
    i686_IDT_SetGate(132, (uint32_t)i686_ISR_132, DEFAULT);
    i686_IDT_SetGate(133, (uint32_t)i686_ISR_133, DEFAULT);
    i686_IDT_SetGate(134, (uint32_t)i686_ISR_134, DEFAULT);
    i686_IDT_SetGate(135, (uint32_t)i686_ISR_135, DEFAULT);
    i686_IDT_SetGate(136, (uint32_t)i686_ISR_136, DEFAULT);
    i686_IDT_SetGate(137, (uint32_t)i686_ISR_137, DEFAULT);
    i686_IDT_SetGate(138, (uint32_t)i686_ISR_138, DEFAULT);
    i686_IDT_SetGate(139, (uint32_t)i686_ISR_139, DEFAULT);
    i686_IDT_SetGate(140, (uint32_t)i686_ISR_140, DEFAULT);
    i686_IDT_SetGate(141, (uint32_t)i686_ISR_141, DEFAULT);
    i686_IDT_SetGate(142, (uint32_t)i686_ISR_142, DEFAULT);
    i686_IDT_SetGate(143, (uint32_t)i686_ISR_143, DEFAULT);
    i686_IDT_SetGate(144, (uint32_t)i686_ISR_144, DEFAULT);
    i686_IDT_SetGate(145, (uint32_t)i686_ISR_145, DEFAULT);
    i686_IDT_SetGate(146, (uint32_t)i686_ISR_146, DEFAULT);
    i686_IDT_SetGate(147, (uint32_t)i686_ISR_147, DEFAULT);
    i686_IDT_SetGate(148, (uint32_t)i686_ISR_148, DEFAULT);
    i686_IDT_SetGate(149, (uint32_t)i686_ISR_149, DEFAULT);
    i686_IDT_SetGate(150, (uint32_t)i686_ISR_150, DEFAULT);
    i686_IDT_SetGate(151, (uint32_t)i686_ISR_151, DEFAULT);
    i686_IDT_SetGate(152, (uint32_t)i686_ISR_152, DEFAULT);
    i686_IDT_SetGate(153, (uint32_t)i686_ISR_153, DEFAULT);
    i686_IDT_SetGate(154, (uint32_t)i686_ISR_154, DEFAULT);
    i686_IDT_SetGate(155, (uint32_t)i686_ISR_155, DEFAULT);
    i686_IDT_SetGate(156, (uint32_t)i686_ISR_156, DEFAULT);
    i686_IDT_SetGate(157, (uint32_t)i686_ISR_157, DEFAULT);
    i686_IDT_SetGate(158, (uint32_t)i686_ISR_158, DEFAULT);
    i686_IDT_SetGate(159, (uint32_t)i686_ISR_159, DEFAULT);
    i686_IDT_SetGate(160, (uint32_t)i686_ISR_160, DEFAULT);
    i686_IDT_SetGate(161, (uint32_t)i686_ISR_161, DEFAULT);
    i686_IDT_SetGate(162, (uint32_t)i686_ISR_162, DEFAULT);
    i686_IDT_SetGate(163, (uint32_t)i686_ISR_163, DEFAULT);
    i686_IDT_SetGate(164, (uint32_t)i686_ISR_164, DEFAULT);
    i686_IDT_SetGate(165, (uint32_t)i686_ISR_165, DEFAULT);
    i686_IDT_SetGate(166, (uint32_t)i686_ISR_166, DEFAULT);
    i686_IDT_SetGate(167, (uint32_t)i686_ISR_167, DEFAULT);
    i686_IDT_SetGate(168, (uint32_t)i686_ISR_168, DEFAULT);
    i686_IDT_SetGate(169, (uint32_t)i686_ISR_169, DEFAULT);
    i686_IDT_SetGate(170, (uint32_t)i686_ISR_170, DEFAULT);
    i686_IDT_SetGate(171, (uint32_t)i686_ISR_171, DEFAULT);
    i686_IDT_SetGate(172, (uint32_t)i686_ISR_172, DEFAULT);
    i686_IDT_SetGate(173, (uint32_t)i686_ISR_173, DEFAULT);
    i686_IDT_SetGate(174, (uint32_t)i686_ISR_174, DEFAULT);
    i686_IDT_SetGate(175, (uint32_t)i686_ISR_175, DEFAULT);
    i686_IDT_SetGate(176, (uint32_t)i686_ISR_176, DEFAULT);
    i686_IDT_SetGate(177, (uint32_t)i686_ISR_177, DEFAULT);
    i686_IDT_SetGate(178, (uint32_t)i686_ISR_178, DEFAULT);
    i686_IDT_SetGate(179, (uint32_t)i686_ISR_179, DEFAULT);
    i686_IDT_SetGate(180, (uint32_t)i686_ISR_180, DEFAULT);
    i686_IDT_SetGate(181, (uint32_t)i686_ISR_181, DEFAULT);
    i686_IDT_SetGate(182, (uint32_t)i686_ISR_182, DEFAULT);
    i686_IDT_SetGate(183, (uint32_t)i686_ISR_183, DEFAULT);
    i686_IDT_SetGate(184, (uint32_t)i686_ISR_184, DEFAULT);
    i686_IDT_SetGate(185, (uint32_t)i686_ISR_185, DEFAULT);
    i686_IDT_SetGate(186, (uint32_t)i686_ISR_186, DEFAULT);
    i686_IDT_SetGate(187, (uint32_t)i686_ISR_187, DEFAULT);
    i686_IDT_SetGate(188, (uint32_t)i686_ISR_188, DEFAULT);
    i686_IDT_SetGate(189, (uint32_t)i686_ISR_189, DEFAULT);
    i686_IDT_SetGate(190, (uint32_t)i686_ISR_190, DEFAULT);
    i686_IDT_SetGate(191, (uint32_t)i686_ISR_191, DEFAULT);
    i686_IDT_SetGate(192, (uint32_t)i686_ISR_192, DEFAULT);
    i686_IDT_SetGate(193, (uint32_t)i686_ISR_193, DEFAULT);
    i686_IDT_SetGate(194, (uint32_t)i686_ISR_194, DEFAULT);
    i686_IDT_SetGate(195, (uint32_t)i686_ISR_195, DEFAULT);
    i686_IDT_SetGate(196, (uint32_t)i686_ISR_196, DEFAULT);
    i686_IDT_SetGate(197, (uint32_t)i686_ISR_197, DEFAULT);
    i686_IDT_SetGate(198, (uint32_t)i686_ISR_198, DEFAULT);
    i686_IDT_SetGate(199, (uint32_t)i686_ISR_199, DEFAULT);
    i686_IDT_SetGate(200, (uint32_t)i686_ISR_200, DEFAULT);
    i686_IDT_SetGate(201, (uint32_t)i686_ISR_201, DEFAULT);
    i686_IDT_SetGate(202, (uint32_t)i686_ISR_202, DEFAULT);
    i686_IDT_SetGate(203, (uint32_t)i686_ISR_203, DEFAULT);
    i686_IDT_SetGate(204, (uint32_t)i686_ISR_204, DEFAULT);
    i686_IDT_SetGate(205, (uint32_t)i686_ISR_205, DEFAULT);
    i686_IDT_SetGate(206, (uint32_t)i686_ISR_206, DEFAULT);
    i686_IDT_SetGate(207, (uint32_t)i686_ISR_207, DEFAULT);
    i686_IDT_SetGate(208, (uint32_t)i686_ISR_208, DEFAULT);
    i686_IDT_SetGate(209, (uint32_t)i686_ISR_209, DEFAULT);
    i686_IDT_SetGate(210, (uint32_t)i686_ISR_210, DEFAULT);
    i686_IDT_SetGate(211, (uint32_t)i686_ISR_211, DEFAULT);
    i686_IDT_SetGate(212, (uint32_t)i686_ISR_212, DEFAULT);
    i686_IDT_SetGate(213, (uint32_t)i686_ISR_213, DEFAULT);
    i686_IDT_SetGate(214, (uint32_t)i686_ISR_214, DEFAULT);
    i686_IDT_SetGate(215, (uint32_t)i686_ISR_215, DEFAULT);
    i686_IDT_SetGate(216, (uint32_t)i686_ISR_216, DEFAULT);
    i686_IDT_SetGate(217, (uint32_t)i686_ISR_217, DEFAULT);
    i686_IDT_SetGate(218, (uint32_t)i686_ISR_218, DEFAULT);
    i686_IDT_SetGate(219, (uint32_t)i686_ISR_219, DEFAULT);
    i686_IDT_SetGate(220, (uint32_t)i686_ISR_220, DEFAULT);
    i686_IDT_SetGate(221, (uint32_t)i686_ISR_221, DEFAULT);
    i686_IDT_SetGate(222, (uint32_t)i686_ISR_222, DEFAULT);
    i686_IDT_SetGate(223, (uint32_t)i686_ISR_223, DEFAULT);
    i686_IDT_SetGate(224, (uint32_t)i686_ISR_224, DEFAULT);
    i686_IDT_SetGate(225, (uint32_t)i686_ISR_225, DEFAULT);
    i686_IDT_SetGate(226, (uint32_t)i686_ISR_226, DEFAULT);
    i686_IDT_SetGate(227, (uint32_t)i686_ISR_227, DEFAULT);
    i686_IDT_SetGate(228, (uint32_t)i686_ISR_228, DEFAULT);
    i686_IDT_SetGate(229, (uint32_t)i686_ISR_229, DEFAULT);
    i686_IDT_SetGate(230, (uint32_t)i686_ISR_230, DEFAULT);
    i686_IDT_SetGate(231, (uint32_t)i686_ISR_231, DEFAULT);
    i686_IDT_SetGate(232, (uint32_t)i686_ISR_232, DEFAULT);
    i686_IDT_SetGate(233, (uint32_t)i686_ISR_233, DEFAULT);
    i686_IDT_SetGate(234, (uint32_t)i686_ISR_234, DEFAULT);
    i686_IDT_SetGate(235, (uint32_t)i686_ISR_235, DEFAULT);
    i686_IDT_SetGate(236, (uint32_t)i686_ISR_236, DEFAULT);
    i686_IDT_SetGate(237, (uint32_t)i686_ISR_237, DEFAULT);
    i686_IDT_SetGate(238, (uint32_t)i686_ISR_238, DEFAULT);
    i686_IDT_SetGate(239, (uint32_t)i686_ISR_239, DEFAULT);
    i686_IDT_SetGate(240, (uint32_t)i686_ISR_240, DEFAULT);
    i686_IDT_SetGate(241, (uint32_t)i686_ISR_241, DEFAULT);
    i686_IDT_SetGate(242, (uint32_t)i686_ISR_242, DEFAULT);
    i686_IDT_SetGate(243, (uint32_t)i686_ISR_243, DEFAULT);
    i686_IDT_SetGate(244, (uint32_t)i686_ISR_244, DEFAULT);
    i686_IDT_SetGate(245, (uint32_t)i686_ISR_245, DEFAULT);
    i686_IDT_SetGate(246, (uint32_t)i686_ISR_246, DEFAULT);
    i686_IDT_SetGate(247, (uint32_t)i686_ISR_247, DEFAULT);
    i686_IDT_SetGate(248, (uint32_t)i686_ISR_248, DEFAULT);
    i686_IDT_SetGate(249, (uint32_t)i686_ISR_249, DEFAULT);
    i686_IDT_SetGate(250, (uint32_t)i686_ISR_250, DEFAULT);
    i686_IDT_SetGate(251, (uint32_t)i686_ISR_251, DEFAULT);
    i686_IDT_SetGate(252, (uint32_t)i686_ISR_252, DEFAULT);
    i686_IDT_SetGate(253, (uint32_t)i686_ISR_253, DEFAULT);
    i686_IDT_SetGate(254, (uint32_t)i686_ISR_254, DEFAULT);
    i686_IDT_SetGate(255, (uint32_t)i686_ISR_255, DEFAULT);
}
