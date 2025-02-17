#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <stdbool.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define LED_RED_PIN     13
#define LED_BLUE_PIN    12
#define LED_GREEN_PIN   11

void led_init(uint8_t pin);
void led_intensity(uint8_t pin, uint8_t intensity);

#endif