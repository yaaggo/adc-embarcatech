#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>


#define SDA_PIN 14
#define SCL_PIN 15

#define I2C_PORT i2c1


#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64


typedef struct {
    uint8_t buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];
    bool initialized;
} display;


void display_init(display *display);
void display_update(display *display);
void display_clear(display *display);
void display_draw_pixel(int x, int y, bool on, display *display);
void display_draw_line(int x0, int y0, int x1, int y1, bool on, display *display);
void display_draw_char(int x, int y, char c, bool on, display *display);
void display_draw_string(int x, int y, const char *str, bool on, display *display);
void display_shutdown(display *display);
void display_draw_rectangle(int x0, int y0, int x1, int y1, bool filled, bool on, display *display);

#endif