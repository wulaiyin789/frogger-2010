/*
 * FroggerProject.c
 *
 * Main file
 *
 * Author: Peter Sutton. Modified by Wu Lai Yin (Peter)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "ledmatrix.h"
#include "scrolling_char_display.h"
#include "pixel_colour.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "score.h"
#include "timer0.h"
#include "game.h"
#include "live.h"
#include "level.h"
#include "timer0.h"
#include "joystick.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void splash_screen(void);
void new_game(void);
void play_game(void);
void next_level(void);
void handle_time_limit(void);
void handle_game_over(void);

// ASCII code for Escape character
#define ESCAPE_CHAR 27
#define INIT_TIME 30

static uint8_t game_over;

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display
	// is complete
	splash_screen();
	
	while(1) {
		new_game();
		while(!game_over) {
			if(no_more_live()) {
				handle_game_over();
			} else {
				next_level();
				play_game();
			}
		}
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	
	init_joystick();
	
	init_lives_display();
	
	// Turn on global interrupts
	sei();
}

void splash_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_cursor(10,10);
	printf_P(PSTR("Frogger"));
	move_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by Wu Lai Yin 44115001"));
	
	// Output the scrolling message to the LED matrix
	// and wait for a push button to be pushed.
	ledmatrix_clear();
	while(1) {
		set_scrolling_display_text("FROGGER 44115001", COLOUR_GREEN);
		// Scroll the message until it has scrolled off the 
		// display or a button is pushed
		while(scroll_display()) {
			_delay_ms(150);
			if(button_pushed() != NO_BUTTON_PUSHED) {
				return;
			}
		}
	}
}

void new_game(void) {
	
	game_over = 0;
	
	// Initialise the game and display
	initialise_game();
	
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the level
	init_level();
	
	// Initialise the score
	init_score();
	
	// Initialise the live
	init_lives();
	
	// Initialise the time
	init_count();
	
	// Start the time clock
	start_counting();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
	
	move_cursor(55,14);
	printf_P(PSTR("Score:%10d"), get_score());
	
	move_cursor(55,15);
	printf_P(PSTR("Lives:%10d"), get_lives());
	
	move_cursor(55,16);
	printf_P(PSTR("Level:%10d"), get_level());
}

void play_game(void) {
	uint32_t current_time;
	
	int8_t joystick;
	int8_t button;
	char serial_input, escape_sequence_char;
	uint8_t characters_into_escape_sequence = 0;
	uint8_t game_paused = 0;
	
	// Get the current time and remember this as the last time the vehicles
	// and logs were moved.
	current_time = get_current_time();
	
	redraw_whole_display();
	
	put_frog_in_start_position();
	
	count_set(INIT_TIME);
	
	// We play the game while the frog is alive and we haven't filled up the 
	// far riverbank
	while(!no_more_live() && !is_riverbank_full()) {
		if(!is_frog_dead() && frog_has_reached_riverbank()) {
			// Frog reached the other side successfully but the
			// riverbank isn't full, put a new frog at the start
			
			add_to_score(10);
			put_frog_in_start_position();
			count_set(INIT_TIME);
		}
		
		if(count_end()) {
			kill_frog();
		}
		
		if(is_frog_dead()) {
			reduce_lives();
			put_frog_in_start_position();
		}
		
		// Check for input - which could be a button push or serial input.
		// Serial input may be part of an escape sequence, e.g. ESC [ D
		// is a left cursor key press. At most one of the following three
		// variables will be set to a value other than -1 if input is available.
		// (We don't initalise button to -1 since button_pushed() will return -1
		// if no button pushes are waiting to be returned.)
		// Button pushes take priority over serial input. If there are both then
		// we'll retrieve the serial input the next time through this loop
		serial_input = -1;
		escape_sequence_char = -1;
		button = button_pushed();
		
		if(button == NO_BUTTON_PUSHED) {
			// No push button was pushed, see if there is any serial input
			if(serial_input_available()) {
				// Serial data was available - read the data from standard input
				serial_input = fgetc(stdin);
				// Check if the character is part of an escape sequence
				if(characters_into_escape_sequence == 0 && serial_input == ESCAPE_CHAR) {
					// We've hit the first character in an escape sequence (escape)
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 1 && serial_input == '[') {
					// We've hit the second character in an escape sequence
					characters_into_escape_sequence++;
					serial_input = -1; // Don't further process this character
				} else if(characters_into_escape_sequence == 2) {
					// Third (and last) character in the escape sequence
					escape_sequence_char = serial_input;
					serial_input = -1;  // Don't further process this character - we
										// deal with it as part of the escape sequence
					characters_into_escape_sequence = 0;
				} else {
					// Character was not part of an escape sequence (or we received
					// an invalid second character in the sequence). We'll process 
					// the data in the serial_input variable.
					characters_into_escape_sequence = 0;
				}
			}
		}
		
		joystick = joystick_direction();
		
		if(!game_paused) {
			// Process the input. 
			if(button==3 || escape_sequence_char=='D' || serial_input=='L' || serial_input=='l' || joystick==3) {
				// Attempt to move left
				move_frog_to_left();
			} else if(button==2 || escape_sequence_char=='A' || serial_input=='U' || serial_input=='u' || joystick==0) {
				// Attempt to move forward
				move_frog_forward();
			} else if(button==1 || escape_sequence_char=='B' || serial_input=='D' || serial_input=='d' || joystick==2) {
				// Attempt to move down
				move_frog_backward();
			} else if(button==0 || escape_sequence_char=='C' || serial_input=='R' || serial_input=='r' || joystick==1) {
				// Attempt to move right
				move_frog_to_right();
			} else {
				switch (can_button_repeat()) {
					case 3:
						move_frog_to_left();
						break;
					case 2:
						move_frog_forward();
						break;
					case 1:
						move_frog_backward();
						break;
					case 0:
						move_frog_to_right();
						break;
				}
			}
		}
		
		
		if(serial_input == 'p' || serial_input == 'P') {
			// Pause/unpause the game until 'p' or 'P' is pressed again
			if(game_paused) {
					game_paused = 0;
					clear_terminal();
					
					move_cursor(55,14);
					printf_P(PSTR("Score:%10d"), get_score());
					
					move_cursor(55,15);
					printf_P(PSTR("Lives:%10d"), get_lives());
					
					move_cursor(55,16);
					printf_P(PSTR("Level:%10d"), get_level());
					
					start_counting();
					
				} else {
					game_paused = 1;
					move_cursor(10,14);
					printf_P(PSTR("GAME PAUSED"));
					
					stop_counting();
				}
		}
		// else - invalid input or we're part way through an escape sequence -
		// do nothing
		
		current_time = get_current_time();
		
		if(!is_frog_dead() && !game_paused) {
			if(current_time % (1000 + (100 * get_level())) == 0) {
				// 1000ms (1 second) has passed since the last time we moved
				// the vehicles and logs - move them again and keep track of
				// the time when we did this.
				scroll_vehicle_lane(0, 1);
				//last_move_time = current_time;
			} else if(current_time % (1100 + (50 * get_level())) == 0) {
				scroll_vehicle_lane(1, -1);
				//last_move_time1 = current_time;
			} else if(current_time % (800 + (50 * get_level())) == 0) {
				scroll_vehicle_lane(2, 1);
				//last_move_time2 = current_time;
			} else if(current_time % (900 + (50 * get_level())) == 0) {
				scroll_river_channel(0, -1);
				//last_move_time3 = current_time;
			} else if(current_time % (1150 + (50 * get_level())) == 0) {
				scroll_river_channel(1, 1);
				//last_move_time4 = current_time;
			}
		}
		displayLED_lives();
	}
	// We get here if the frog is dead or the riverbank is full
	// The game is over.
}

void next_level(void) {
	count_clear();
	add_level();
	if(get_level() > 1) {
		add_lives();
	}
	
	clear_terminal();
	move_cursor(55,16);
	printf_P(PSTR("Level:%10d"), get_level());
	
	move_cursor(55,15);
	printf_P(PSTR("Lives:%10d"), get_lives());
	
	move_cursor(55,14);
	printf_P(PSTR("Score:%10d"), get_score());
	
	ledmatrix_clear();
	
	char level_txt[8];
	sprintf(level_txt, "LEVEL %i", get_level());
	set_scrolling_display_text(level_txt, COLOUR_YELLOW);
	
	while(scroll_display()) {
		if (button_pushed() != NO_BUTTON_PUSHED) {
			initialise_game();
			break;
		}
		_delay_ms(150);
	}
}


void handle_game_over() {
	game_over = 1;
	count_clear();
	ledmatrix_clear();
	
	move_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));

	while(1) {
		set_scrolling_display_text("GAME OVER", COLOUR_GREEN);
		while(scroll_display()) {
			_delay_ms(170);
			if(button_pushed() != NO_BUTTON_PUSHED) {
				return;
			}
		}
	}
}