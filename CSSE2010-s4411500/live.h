/*
 * live.h
 * 
 * Author: Wu Lai Yin (Peter)
 */

#ifndef LIVES_H_
#define LIVES_H_

#include <stdint.h>

void init_lives_display(void);
void init_lives(void);
void add_lives(void);
void reduce_lives(void);
uint8_t no_more_live(void);
uint8_t get_lives(void);
void displayLED_lives(void);

#endif /* LIVES_H_ */