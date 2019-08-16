/*
 * game.c
 *
 * Author: Wu Lai Yin (Peter)
 */ 

#include <stdio.h>
#include <avr/interrupt.h>

#include "joystick.h"
#include "timer0.h"

static uint16_t x_value;
static uint16_t y_value;
static uint8_t old_direction;
static uint32_t old_time;

void init_joystick(void) {
	// Set up ADC - AVCC reference, right adjust
	ADMUX = (1 << REFS0);
		
	// Turn on the ADC
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

void adc_values(void) {
	// x axis 
	// ADC6
	ADMUX = 0b01000110;
	// Start the ADC conversion
	ADCSRA |= (1 << ADSC);
		
	while(ADCSRA & (1 << ADSC)) {
		; /* Wait until conversion finished */
	}
	
	x_value = ADC; // read the value
	
	// y axis
	// ADC7
	ADMUX = 0b01000111;
	// Start the ADC conversion
	ADCSRA |= (1 << ADSC);
		
	while(ADCSRA & (1 << ADSC)) {
		; /* Wait until conversion finished */
	}
	
	y_value = ADC; // read the value
}

int8_t joystick_direction(void) {
	
	/* 3 = LEFT
	 * 2 = DOWN
	 * 1 = RIGHT
	 * 0 = UP
	 * -1 = MIDDLE
	 */
	
	uint8_t new_direction;
	uint32_t current_time;
	
	adc_values();
	
	if(x_value < 250) {
		new_direction = 3;
	} else if (y_value < 250) {
		new_direction = 2;
	} else if (x_value > 760) {
		new_direction = 1;
	} else if (y_value > 760) {
		new_direction = 0;
	} else {
		new_direction = -1;
	}
	
	if(new_direction >= 0) {
		if(old_direction == new_direction) {
			current_time =  get_current_time();
			if(current_time < old_time + 250) {
				return -1;
			}
		}
			
		old_direction = new_direction;
		old_time =  get_current_time();
		return new_direction;
	} else {
		return -1;
	}
}