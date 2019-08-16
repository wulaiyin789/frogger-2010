/*
 * score.c
 *
 * Written by Peter Sutton
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "score.h"
#include "terminalio.h"

uint8_t score;

void init_score(void) {
	score = 0;
}

void add_to_score(uint16_t value) {
	score += value;
	
	move_cursor(55,14);
	printf_P(PSTR("Score:%10d"), get_score());
}

uint32_t get_score(void) {
	return score;
}
