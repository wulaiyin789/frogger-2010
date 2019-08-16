/*
 * level.c
 *
 * Written by Wu Lai Yin (Peter)
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "level.h"
#include "live.h"
#include "score.h"
#include "buttons.h"
#include "game.h"
#include "timer0.h"
#include "terminalio.h"
#include "ledmatrix.h"
#include "scrolling_char_display.h"

#define F_CPU 8000000L
#include <util/delay.h>

uint8_t level;

void init_level(void) {
	level = 0;
}

void add_level(void) {
	level++;
}

uint8_t get_level(void) {
	return level;
}