/*
 * timer0.c
 *
 * Author: Peter Sutton
 *
 * We setup timer0 to generate an interrupt every 1ms
 * We update a global clock tick variable - whose value
 * can be retrieved using the get_clock_ticks() function.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer0.h"

/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clockTicks;

static volatile uint32_t timeClockTicks;

static uint16_t timer_count = 0;

static volatile uint16_t count = 0;

static volatile uint8_t digit_counter = 0;

/* Seven segment display segment values for 0 to 9 */
static const uint8_t seven_seg_data[10] = {63,6,91,79,102,109,125,7,127,111};

/* Set up timer 0 to generate an interrupt every 1ms. 
 * We will divide the clock by 64 and count up to 124.
 * We will therefore get an interrupt every 64 x 125
 * clock cycles, i.e. every 1 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer0(void) {
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	clockTicks = 0L;
	timeClockTicks = 0L;
	
	/* Clear the timer */
	TCNT0 = 0;

	/* Set the output compare value to be 124 */
	OCR0A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01)|(1<<CS00);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK0 |= (1<<OCIE0A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR0 &= (1<<OCF0A);
}

uint32_t get_current_time(void) {
	uint32_t returnValue;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();
	returnValue = clockTicks;
	if(interruptsOn) {
		sei();
	}
	return returnValue;
}

uint32_t get_time_clock_ticks(void) {
	uint32_t returnValue;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();
	returnValue = clockTicks;
	if(interruptsOn) {
		sei();
	}
	return returnValue;
}

void start_counting(void) {
	timer_count = 1;
}

void stop_counting(void) {
	timer_count = 0;
}

void init_count(void) {
	DDRC = 0xFF;
	DDRD |= (1 << DDRD2);
	
	count = 0;
}

void count_set(uint8_t start) {
	count = start * 1000;
}

void count_clear(void) {
	count = 0;
}

uint8_t count_end(void) {
	return (count == 0);
}

ISR(TIMER0_COMPA_vect) {
	clockTicks++;
	
	if(timer_count) {
		timeClockTicks++;
		if(count > 0) {
			count--;
		}
	}
	
	digit_counter++;
	if(digit_counter > 3) {
		digit_counter = 0;
	}
	
	uint8_t seven_seg_cc = digit_counter >> 1;
	
	//  Start countdown when game started
	if(count > 0) { 
		uint16_t to_display = count + 1000;
		//led display
		if(seven_seg_cc == 0) {
			/* Display rightmost digit - tens of second */
			PORTC = seven_seg_data[(to_display / 1000) % 10];
			
		} else {
			/* Display leftmost digit - tens of seconds */
			if (to_display > 10000) { 
				PORTC = seven_seg_data[(to_display / 10000) % 10];
			} else {
				PORTC = 0;
			}
			
		}
	} else {
		PORTC = 0;
	}
	
	/* Output the digit selection (CC) bit */
	if (seven_seg_cc) {
		PORTD |= (1 << PORTD2);
	} else {
		PORTD &= ~(1 << PORTD2);
	}
}
