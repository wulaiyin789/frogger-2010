/*
 * live.c
 *
 * Written by Wu Lai Yin (Peter)
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "live.h"
#include "terminalio.h"


uint8_t lives = 0;
uint8_t initial_lives = 3;
uint8_t max_lives = 4;

void init_lives_display(void) {
	DDRA |= 0x0F;
	PORTA &= 0xF0;
}

void init_lives(void) {
	lives = initial_lives;
	
	displayLED_lives();
}

void add_lives(void) {
	if(lives < max_lives) {
		lives++;
	}
	
	displayLED_lives();
}

void reduce_lives(void) {
	if(lives > 0) {
		lives--;
	}
	
	move_cursor(55,15);
	printf_P(PSTR("Lives:%10d"), get_lives());
	
	displayLED_lives();
}

uint8_t no_more_live(void) {
	return (lives == 0);
}

uint8_t get_lives(void) {
	return lives;
}

void displayLED_lives(void) {
	
	/* A0 - A3 are outputs
	 * A0 -> L0
	 * A1 -> L1
	 * A2 -> L2
	 * A3 -> L3
	 */
	
	PORTA &= 0xF0;
	
	for(uint8_t i = 0; i < lives;  i++) {
		PORTA |= (1 << i);
	}
}
