#include "display.h"
#include "font.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>

static void i2c_init_custom();
static void ssd1306_send_command(uint8_t command);

static void i2c_init_custom() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

static void ssd1306_send_command(uint8_t command) {
    uint8_t data[] = {0x00, command};
    i2c_write_blocking(I2C_PORT, 0x3C, data, sizeof(data), false);
}
// inicializa tudo do display
void display_init(display *display) {
    if (display->initialized) return;

    i2c_init_custom();

    ssd1306_send_command(0xAE); // display OFF
    ssd1306_send_command(0xD5); // Set display Clock Divide Ratio
    ssd1306_send_command(0x80); // Frequência padrão
    ssd1306_send_command(0xA8); // Set Multiplex Ratio
    ssd1306_send_command(0x3F); // Multiplex para 64 linhas
    ssd1306_send_command(0xD3); // Set display Offset
    ssd1306_send_command(0x00); // Sem deslocamento
    ssd1306_send_command(0x40); // Set display Start Line para 0
    ssd1306_send_command(0x8D); // Ativa Charge Pump
    ssd1306_send_command(0x14); // Habilita
    ssd1306_send_command(0x20); // Define modo de endereçamento
    ssd1306_send_command(0x00); // Modo horizontal
    ssd1306_send_command(0xA1); // Segment Re-map (coluna 127 mapeada para SEG0)
    ssd1306_send_command(0xC8); // COM Output Scan Direction (invertido)
    ssd1306_send_command(0xDA); // Set COM Pins Hardware Configuration
    ssd1306_send_command(0x12); // Configuração padrão
    ssd1306_send_command(0x81); // Define contraste
    ssd1306_send_command(0x7F); // Valor de contraste (127)
    ssd1306_send_command(0xD9); // Define período de pré-carga
    ssd1306_send_command(0xF1); // Valor padrão
    ssd1306_send_command(0xDB); // Define nível de deseleção VCOMH
    ssd1306_send_command(0x40); // Nível padrão
    ssd1306_send_command(0xA4); // Define display como "seguindo o conteúdo da RAM"
    ssd1306_send_command(0xA6); // display em modo normal (não invertido)
    ssd1306_send_command(0xAF); // display ON

    memset(display->buffer, 0, sizeof(display->buffer));
    display->initialized = true;
}

// coloca dados do buffer no display
void display_update(display *display) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(0xB0 + page);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);

        uint8_t data[129];
        data[0] = 0x40;
        memcpy(&data[1], &display->buffer[page * DISPLAY_WIDTH], DISPLAY_WIDTH);

        i2c_write_blocking(I2C_PORT, 0x3C, data, sizeof(data), false);
    }
}

// limpa o buffer do display
void display_clear(display *display) {
    memset(display->buffer, 0, sizeof(display->buffer));
}

// desenha ou apaga um pixel no display
void display_draw_pixel (int x, int y, bool on, display *display) {
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT) return; 
    if (on)
        display->buffer[x + (y / 8) * DISPLAY_WIDTH] |= (1 << (y % 8));
    else
        display->buffer[x + (y / 8) * DISPLAY_WIDTH] &= ~(1 << (y % 8));
}

// algoritmo de Bresenham
void display_draw_line(int x0, int y0, int x1, int y1, bool on, display *display) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        display_draw_pixel(x0, y0, on, display);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy)  err += dy, x0 += sx;
        if (e2 <= dx)  err += dx, y0 += sy;
    }
}

// escreve um caracter no buffer do display
void display_draw_char(int x, int y, char c, bool on, display *display) {
    if (c < 0x20 || c > 0x7F) return;

    int index = c - 0x20;

    for (int col = 0; col < 8; col++) {
        uint8_t line = FONTS[index][col];
        
        for (int row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                display_draw_pixel(x + col, y + row, on, display);
            }
        }
    }
}

// escreve uma string no buffer do display
void display_draw_string(int x, int y, const char *str, bool on, display *display) {
    while (*str) {
        if (x + 8 > 128) break;

        display_draw_char(x, y, *str, on, display);
        x += 8;
        str++;
    }
}

void display_draw_rectangle(int x0, int y0, int x1, int y1, bool filled, bool on, display *display) {
    // Ajusta os valores para caberem na tela de forma "circular" estilo snake
    x0 = (x0 + DISPLAY_WIDTH) % DISPLAY_WIDTH;
    y0 = (y0 + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;
    x1 = (x1 + DISPLAY_WIDTH) % DISPLAY_WIDTH;
    y1 = (y1 + DISPLAY_HEIGHT) % DISPLAY_HEIGHT;

    // Se for um retângulo preenchido
    if (filled) {
        for (int y = y0; y != (y1 + 1) % DISPLAY_HEIGHT; y = (y + 1) % DISPLAY_HEIGHT) {
            for (int x = x0; x != (x1 + 1) % DISPLAY_WIDTH; x = (x + 1) % DISPLAY_WIDTH) {
                display_draw_pixel(x, y, on, display);
            }
        }
    } else {
        // Desenha apenas as bordas do retângulo
        display_draw_line(x0, y0, x1, y0, on, display); // Linha superior
        display_draw_line(x0, y1, x1, y1, on, display); // Linha inferior
        display_draw_line(x0, y0, x0, y1, on, display); // Linha esquerda
        display_draw_line(x1, y0, x1, y1, on, display); // Linha direita
    }
}

// apaga e desliga o display
void display_shutdown(display *display) {

    display_clear(display);
    display_update(display);

    ssd1306_send_command(0xAE); // display OFF

    ssd1306_send_command(0x8D); // charge Pump
    ssd1306_send_command(0x10); // desativa

    i2c_deinit(I2C_PORT);
    gpio_set_function(SDA_PIN, GPIO_FUNC_NULL);
    gpio_set_function(SCL_PIN, GPIO_FUNC_NULL);

    display->initialized = false;
}

