/*
 * game.h
 *
 * Author: Peter Sutton
 *
 * The game has 8 rows, numbered 0 to 7 from the bottom. 
 * The frog starts in row 0 (safe) and has to cross 3 lanes
 * of traffic (rows 1 to 3) without being hit by any traffic
 * before arriving at the other side of the road (row 4) 
 * where it is safe. It then has to cross a river by jumping
 * on to logs (rows 5 and 6) before jumping into into a hole
 * on the riverbank (row 7).
 *
 * The functions in this module will update the LED matrix
 * display as required. 
 */ 

#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

// Reset the game. Get the road and river ready and place a frog
// on the roadside (bottom row)
void initialise_game(void);

// Add a frog to the game in the starting (bottom) row
// (This would typically be called after a frog has made it 
// successfully to the other side.)
void put_frog_in_start_position(void);

/////////////////////////////////// MOVE FUNCTIONS /////////////////////////
// is_frog_dead() should be checked after calling one of these to see
// if the move succeeded or not

// Move the frog one row forward.
// This function must NOT be called if the frog is in row 7 (i.e. home).
// Failure may occur if the frog jumps into a vehicle or jumps in the water 
// or jumps into the riverbank. 
void move_frog_forward(void);

// Move the frog one row backward, if possible.
void move_frog_backward(void);

// Move the frog one column left. 
// Failure may occur if the frog jumps into a vehicle or jumps off a log
// into the river. Attempts to jump off the game field result in the frog dying.
void move_frog_to_left(void);

// Move the frog one column right.
// Failure may occur if the frog jumps into a vehicle or jumps off a log
// into the river. Attempts to jump off the game field result in the frog dying. 
void move_frog_to_right(void);

/////////////////////// FROG / GAME STATUS ///////////////////////////////////
// Return the position of the frog. The row ranges from 0 (bottom) to 7 (top).
// The column ranges from 0 (left hand side) to 1 (right hand side)
uint8_t get_frog_row(void);
uint8_t get_frog_column(void);

// Check whether the destination riverbank is full (i.e. there are frogs 
// in all the holes).
uint8_t is_riverbank_full(void);

// Check whether the frog has reached the riverbank (the other side).
// (If this returns true, the frog should not be moved any further.)
uint8_t frog_has_reached_riverbank(void);

// Check whether the frog is alive or dead
uint8_t is_frog_dead(void);

// Kill the frog immediately
void kill_frog(void);

/////////////////////// UPDATE FUNCTIONS /////////////////////////////////////
// Scroll the given lane of traffic in the given direction. 
// Check is_frog_dead() to determine whether the frog was killed or not.
// lane argument is 0, 1 or 2 corresponding to rows 1, 2 and 3 on the display.
// direction argument is -1 for left, 1 for right, 0 for no scroll (just redraw)
void scroll_vehicle_lane(uint8_t lane, int8_t direction);

// Scroll the given log channel (and the frog if the frog is on a log) in
// the given direction.
// Check is_frog_dead() to determine whether the frog was killed or not.
// (Frog dies if it hits the edge of the game field whilst on a log.)
// log argument is 0 or 1 (corresponding to rows 5 and 6 on the display).
// direction argument is -1 for left, 1 for right, 0 for no scroll (just redraw)
void scroll_river_channel (uint8_t channel, int8_t direction);

void redraw_whole_display(void);

#endif /* GAME_H_ */