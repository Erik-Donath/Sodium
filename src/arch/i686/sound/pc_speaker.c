#include "pc_speaker.h"

#include "../cpu/io.h"

#include <stdint.h>

// Internal

#define PIT_CHANNEL2   0x42
#define PIT_CMD        0x43
#define SPEAKER_PORT   0x61

#define PIT_BASE_FREQ  1193180u

// @brief Program PIT channel 2 with a frequency divisor.
static void i686_pcspeaker_set_freq(uint32_t frequency);

// Definitions

static void i686_pcspeaker_set_freq(uint32_t frequency) {
    uint32_t divisor = PIT_BASE_FREQ / frequency;
    i686_io_outb(PIT_CMD,      0xB6);
    i686_io_outb(PIT_CHANNEL2, (uint8_t)(divisor & 0xFF));
    i686_io_outb(PIT_CHANNEL2, (uint8_t)(divisor >> 8));
}

void i686_pcspeaker_play_sound(uint32_t nFrequence) {
    i686_pcspeaker_set_freq(nFrequence);
    uint8_t tmp = i686_io_inb(SPEAKER_PORT);
    if ((tmp & 0x03) != 0x03)
        i686_io_outb(SPEAKER_PORT, tmp | 0x03);
}

void i686_pcspeaker_stop_sound(void) {
    uint8_t tmp = i686_io_inb(SPEAKER_PORT);
    i686_io_outb(SPEAKER_PORT, tmp & ~0x03);
}

void i686_pcspeaker_beep(void) {
    i686_pcspeaker_play_sound(1000);
    for (int x = 0; x < 4000000; x++) i686_io_wait();
    i686_pcspeaker_stop_sound();
}
