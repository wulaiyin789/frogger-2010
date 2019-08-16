/*
 * live.h
 * 
 * Author: Wu Lai Yin (Peter)
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

void init_joystick(void);
void adc_valves(void);

int8_t joystick_direction(void);

#endif /* JOYSTICK_H_ */