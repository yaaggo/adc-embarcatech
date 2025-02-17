#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_BUTTON_PIN 22

#define DEADZONE 100

void joystick_init(uint8_t pin_x, uint8_t pin_y);
int16_t joystick_read(uint8_t pin, uint16_t deadzone, uint16_t limit);

#endif