#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"

// Definição dos pinos dos LEDs RGB
#define LED_GREEN_PIN 11
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13

// Definição dos pinos dos botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Definição do pino do WS2812 e número total de LEDs na matriz
#define WS2812_PIN 7
#define NUM_LEDS 25

// Tempo de debounce para os botões (evita múltiplas detecções)
#define DEBOUNCE_DELAY_MS 50

// Brilho dos LEDs (0 a 255)
#define BRIGHTNESS 1 

// Variável para armazenar o número a ser exibido
static volatile int counter = 0;

// Inicialização do periférico PIO e do estado da máquina de estado
PIO pio = pio0;
int sm = 0;

// Array que armazena os valores RGB dos LEDs
uint32_t leds[NUM_LEDS];

// Definição da matriz de números de 0 a 9 para exibição nos LEDs
const bool digits[10][25] = {
    { // 0
        1,1,1,1,1,
        1,0,0,0,1,
        1,0,0,0,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    { // 1
        0,0,1,0,0,
        0,1,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,1,1,1,0
    },
    { // 2
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1
    },
    { // 3
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    },
    { // 4
        1,0,0,0,1,
        1,0,0,0,1,
        1,1,1,1,1,
        0,0,0,0,1,
        0,0,0,0,1
    },
    { // 5
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    },
    { // 6
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    { // 7
        1,1,1,1,1,
        0,0,0,0,1,
        0,0,0,1,0,
        0,0,1,0,0,
        0,1,0,0,0
    },
    { // 8
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    { // 9
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    }
};

// Função para criar cores RGB com ajuste de brilho
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
    return ((uint32_t)(g * brightness / 255) << 8) | ((uint32_t)(r * brightness / 255) << 16) | (uint32_t)(b * brightness / 255);
}

// Mapeamento da ordem dos LEDs conforme a matriz física
const int led_map[25] = {
    24, 23, 22, 21, 20,
    15, 16, 17, 18, 19,
    14, 13, 12, 11, 10,
    5, 6, 7, 8, 9,
    4, 3, 2, 1, 0
};

// Atualiza a exibição do número nos LEDs
void update_display(int digit) {
    for (int i = 0; i < 25; i++) {
        int mapped_index = led_map[i];
        leds[mapped_index] = digits[digit][i] ? urgb_u32(255, 255, 255, BRIGHTNESS) : 0;
    }

    for (int i = 0; i < 25; i++) {
        pio_sm_put_blocking(pio, sm, leds[i] << 8u);
    }
}

// Callback para debounce do botão
static int64_t debounce_callback(alarm_id_t id, void *user_data) {
    uint gpio = (uint)user_data;
    if (gpio_get(gpio) == 0) {
        if (gpio == BUTTON_A_PIN) {
            counter = (counter + 1) % 10;
        } else if (gpio == BUTTON_B_PIN) {
            counter = (counter - 1 + 10) % 10;
        }
        update_display(counter);
    }
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, true);
    return 0;
}

// Interrupção do botão
void button_isr(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, false);
    add_alarm_in_ms(DEBOUNCE_DELAY_MS, debounce_callback, (void*)gpio, false);
}

// Timer para piscar o LED vermelho
bool blink_timer_callback(struct repeating_timer *t) {
    gpio_put(LED_RED_PIN, !gpio_get(LED_RED_PIN));
    return true;
}

int main() {
    stdio_init_all();

    // Configuração dos LEDs RGB
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);

    // Configuração dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Inicializa o WS2812 via PIO
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &button_isr);

    // Configuração do timer para piscar LED
    struct repeating_timer timer;
    add_repeating_timer_ms(100, blink_timer_callback, NULL, &timer);

    // Inicia o display mostrando "0"
    update_display(0);

    // Loop principal
    while (true) {
        tight_loop_contents();
    }

    return 0;
}
