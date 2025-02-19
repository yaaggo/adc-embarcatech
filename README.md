# Controle de servomotor por PWM.

## Enunciado
  Com o objetivo de consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no
RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab, propõe-se a realização da
seguinte atividade prática individual.


---

## Resquisitos
1) Uso de interrupções: Todas as funcionalidades relacionadas aos botões devem ser implementadas utilizando rotinas de interrupção (IRQ).
2) Debouncing: É obrigatório implementar o tratamento do bouncing dos botões via software.
3) Utilização do Display 128 x 64: A utilização de ferramentas gráficas demonstrará o entendimento do princípio de funcionamento do display, bem como, a utilização do protocolo I2C.
4) Organização do código: O código deve estar bem estruturado e comentado para facilitar o entendimento.

---

## Ferramentas utilizadas
- LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
- Botão do Joystick conectado à GPIO 22.
- Joystick conectado aos GPIOs 26 e 27.
- Botão A conectado à GPIO 5.
- Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).

---

## Funcionamento do Programa
- Aparecerá um quadrado de tamanho 8 x 8 no centro da tela, do qual tem seu movimento sincronizado com a manipulação do joystick.
- Os LEDs vermelho e azul estão sincronizados ao movimento do joystick, utilizando o pwm para alteração suave na intensidade.
  - O LED vermelho esta atrelado ao eixo X do joystick.
  - o LED azul esta conectado ao eixo y do joystick.
- Apertar o botão do joystick ativará o LED verde.
- Apertar o botão A desligará os LEDs que variam com PWM.

---

## Estrutura do Código
- Foi usado bibliotecas autorais para a manipulação das ferramentas da placa bitdoglab.
  - joystick: inicializar e retornar o valor dos joysticks utilizando ADC.
  - display: inicializar e manipular a tela ssd1306
  - led: inicializa e controla a luminosidade dos LEDs utilizando PWM.
- Na Main esta a lógica do debouncing, interrupção, manipulação dos leds e tudo mais.

---

## Principais funções e arquivos
- Função para desenhar retangulos no display, usada para fazer o quadrado e as molduras.
```c
void display_draw_rectangle(int x0, int y0, int x1, int y1, bool filled, bool on, display *display);
```

- Função para inicializar um joystick, inicializando o adc e pegando o valor imediato para settar o centro.
```c
void joystick_init(uint8_t pin_x, uint8_t pin_y);
```

- Função para ler o valor de um eixo do joystick, considerando uma deadzone e um limite no calculo.
```c
int16_t joystick_read(uint8_t pin, uint16_t deadzone, uint16_t limit);
```

- Função para controlar a intensidade do LED utilizando PWM.
```c
void led_intensity(uint8_t pin, uint8_t intensity);
```

- Função de callback para ações com os botões.
```c
void button_callback(uint gpio, uint32_t events);
```

---

## Como Executar o Projeto

1) **Configurar o ambiente**:
   - Instale o Raspberry Pi Pico SDK.
   - Configure o ambiente de desenvolvimento.

2) **Compilar o código**:
   - Clone o repositório no diretório que você quer compilar utilizando:
     ```bash
     git clone https://github.com/yaaggo/adc-embarcatech.git
     ```
   - Compile utilizando as ferramentas do SDK do Pico.
   - (opcional) Se estiver utilizando a extensão do Raspberry Pi Pico SDK no vscode, importe o projeto e compile pela extensão

4) **Fazer upload para a placa**:
   - Conecte a Raspberry Pi Pico via USB.
   - Envie o binário gerado para a placa.

5) **Executar o código**:
   - Apenas dar play no simulador e observar o servomotor se movendo.
   - No caso da placa, utilizando o LED, apenas observar também.
---
## Video demonstrativo
  Link do video com uma explicação curta e demonstrando as features na placa: https://www.youtube.com/watch?v=MMn-4MMns3U
