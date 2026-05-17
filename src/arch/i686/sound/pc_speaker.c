#include "pc_speaker.h"
#include "../cpu/io.h"

// Heavaly inspired by https://wiki.osdev.org/PC_Speaker

void i686_pcspeaker_play_sound(uint32_t nFrequence) {
    uint32_t Div;
	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	i686_io_outb(0x43, 0xb6);
 	i686_io_outb(0x42, (uint8_t) (Div) );
 	i686_io_outb(0x42, (uint8_t) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = i686_io_inb(0x61);
  	if (tmp != (tmp | 3)) {
 	    i686_io_outb(0x61, tmp | 3);
 	}
}

void i686_pcspeaker_stop_sound() {
 	uint8_t tmp = i686_io_inb(0x61) & 0xFC;
 	i686_io_outb(0x61, tmp);
}

void i686_pcspeaker_beep() {
 	i686_pcspeaker_play_sound(1000);
 	for(int x = 0; x < 4000000; x++)
        i686_io_wait();
 	i686_pcspeaker_stop_sound();
}
