#pragma once

#include <stdint.h>

// @brief Start the PC speaker at the given frequency.
//        Sound plays until i686_pcspeaker_stop_sound() is called.
// @param nFrequence  Desired frequency in Hz.
void i686_pcspeaker_play_sound(uint32_t nFrequence);

// @brief Stop the PC speaker by clearing the gate on port 0x61.
void i686_pcspeaker_stop_sound(void);

// @brief Play a 1000 Hz beep for a fixed duration.
//        #FIXME: duration is an I/O busy-wait loop, not calibrated to time.
//              Replace with a timer-based delay once scheduler ticks exist.
void i686_pcspeaker_beep(void);
