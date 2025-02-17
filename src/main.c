#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "include/joystick.h"
#include "include/display.h"
#include "include/led.h"

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define DEBOUNCE_DELAY 200

#define DEBUG(var) printf("%s: %d\n", #var, var);

void button_callback(uint gpio, uint32_t events);
void button_init(uint8_t pin);

display dp;

static volatile uint32_t last_a_interrupt_time = 0;
static volatile uint32_t last_b_interrupt_time = 0;
static volatile uint32_t last_joystick_interrupt_time = 0;

volatile bool led_green_state = 0;
volatile bool button_a_state = 0;


int main() {
    stdio_init_all();
    // inicializando os leds
    led_init(LED_BLUE_PIN);
    led_init(LED_GREEN_PIN);
    led_init(LED_RED_PIN);

    // inicializando o joystick
    joystick_init(JOYSTICK_X_PIN, JOYSTICK_Y_PIN);

    // inicializando o display
    uint8_t cube_x = DISPLAY_WIDTH / 2 - 8;
    uint8_t cube_y = DISPLAY_HEIGHT / 2 - 8;
    uint8_t previous_cube_x = cube_x;
    uint8_t previous_cube_y = cube_y;


    display_clear(&dp);
    display_init(&dp);

    display_draw_rectangle(0, 0, 127, 63, false, true, &dp);
    display_draw_rectangle(cube_x, cube_y, cube_x + 8, cube_y + 8, true, true, &dp);

    display_update(&dp);



    // inicializando botoes
    button_init(BUTTON_A_PIN);
    button_init(BUTTON_B_PIN);
    button_init(JOYSTICK_BUTTON_PIN);

    while (true) {
        int16_t joystick_x = joystick_read(JOYSTICK_X_PIN, 10, 510);
        int16_t joystick_y = joystick_read(JOYSTICK_Y_PIN, 10, 510);
        

        if(!button_a_state) {
            led_intensity(LED_BLUE_PIN, abs(joystick_y));
            led_intensity(LED_RED_PIN, abs(joystick_x));
            led_intensity(LED_GREEN_PIN, led_green_state * 255);
        } else {
            led_intensity(LED_BLUE_PIN, 0);
            led_intensity(LED_RED_PIN, 0);
            led_intensity(LED_GREEN_PIN, 0);

        }

        joystick_x = ((joystick_x * 10) / 510);
        joystick_y = ((joystick_y * 10) / 510) * (-1);

        DEBUG(joystick_x);
        DEBUG(joystick_y);
        
        cube_x = (cube_x + joystick_x + DISPLAY_WIDTH) % DISPLAY_WIDTH;
        cube_y = (cube_y + joystick_y + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;

        if(previous_cube_x != cube_x || previous_cube_y != cube_y) {
            display_draw_rectangle(previous_cube_x, previous_cube_y, previous_cube_x + 8, previous_cube_y + 8, true, false, &dp);
            display_draw_rectangle(cube_x, cube_y, cube_x + 8, cube_y + 8, true, true, &dp);
            previous_cube_x = cube_x;
            previous_cube_y = cube_y;
        }
        display_draw_rectangle(0, 0, 127, 63, false, true, &dp);   
        display_update(&dp);
        sleep_ms(30);
    }

    return 0;
}

void button_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());

    // verifica qual botão acionou a interrupção e trata o debounce
    if (gpio == BUTTON_A_PIN) { // botão do led verde
        if (current_time - last_a_interrupt_time > DEBOUNCE_DELAY) {
            last_a_interrupt_time = current_time;

            if (events & GPIO_IRQ_EDGE_FALL) {
                button_a_state = !button_a_state;
            }
        }
    } else if (gpio == BUTTON_B_PIN) { // botao extra para entrar em modo bootsel
        if (current_time - last_b_interrupt_time > DEBOUNCE_DELAY) {
            last_b_interrupt_time = current_time;

            if (events & GPIO_IRQ_EDGE_FALL) {   
                display_shutdown(&dp);
                reset_usb_boot(0, 0);
            }
        }
    } else if (gpio == JOYSTICK_BUTTON_PIN) { 
        if (current_time - last_joystick_interrupt_time > DEBOUNCE_DELAY) {
            last_joystick_interrupt_time = current_time;

            if (events & GPIO_IRQ_EDGE_FALL) {   
                led_green_state = !led_green_state;
            }
        }
    }
}

// inicializa o botão e configura interrupções
void button_init(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);

    // habilita interrupções para bordas de subida e descida
    gpio_set_irq_enabled_with_callback(
        pin,
        GPIO_IRQ_EDGE_FALL,
        true,
        &button_callback
    );
}