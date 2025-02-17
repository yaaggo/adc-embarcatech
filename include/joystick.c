#include "joystick.h"

// variaveis estaticas para guardar o valor do centro
// no momento da inicialização
static uint16_t center_x = 0;
static uint16_t center_y = 0;

void joystick_init(uint8_t pin_x, uint8_t pin_y) {

    // inicializando os adc dos pinos 
    adc_init();
    adc_gpio_init(pin_x);
    adc_gpio_init(pin_y);
    
    // calibrar os valores centrais
    adc_select_input(pin_x - 26);
    center_x = adc_read();
    adc_select_input(pin_y - 26);
    center_y = adc_read();
}

int16_t joystick_read(uint8_t pin, uint16_t deadzone, uint16_t limit) {
    // seleciona de qual adc irão vir os inputs
    adc_select_input(pin - 26);

    // leio o valor correspondente ao adc selecionado
    uint16_t value = adc_read();

    // escolho qual centro irei usar baseado no pino
    uint16_t center = (pin == JOYSTICK_X_PIN) ? center_x : center_y;
    
    // lógica para a deadzone funcionar baseado no valor de limit
    int16_t adjusted_value = (int16_t)value - (int16_t)center;
    int16_t scaled_deadzone = (deadzone * 4095) / limit;
    
    if (adjusted_value > -scaled_deadzone && adjusted_value < scaled_deadzone) {
        return 0;
    }
    
    // como limit é o maximo de valores que queremos mapear
    // dividindo por 2 temos o range maximo que ele vai percorrer na linha dos inteiros
    // entre -range <- 0 -> range
    int16_t range = (limit / 2);
    
    // como adjusted_value é a leitura do adc menos o centro
    // fazemos a escolha dependendo do sinal dele
    if (adjusted_value < 0) {
        // normalizando levando em consideração o center
        return (adjusted_value * range) / center; // center é o valor máximo para o lado dos negativos
    } else {
        // normalizando levando em consideração (4095 - center)
        return (adjusted_value * range) / (4095 - center);//(4095 - center) é o valor máximo para o lado dos positivos
    }
}
