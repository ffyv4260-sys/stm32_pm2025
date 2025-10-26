#define LED_PIN       13     // Светодиод на PC13
#define BTN_UP_PIN     0     // Кнопка "A" — PA0
#define BTN_DOWN_PIN   1     // Кнопка "B" — PC1

// Начальная частота мигания (1 Гц)
volatile float freq = 1.0f;
volatile uint32_t delay_ms = 500; // половина периода (1 Гц -> 500 мс)

// Пределы частоты
#define FREQ_MAX 64.0f
#define FREQ_MIN (1.0f / 64.0f)

// Простая программная задержка
void delay_ms_func(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

// Инициализация портов
void gpio_init(void) {
    // Включаем тактирование портов A и C
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

    // PC13 — выход push-pull, 2 MHz
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    GPIOC->CRH |= GPIO_CRH_MODE13_1; // MODE13 = 10 (2 MHz), CNF13 = 00 (push-pull)

    // PA0 — вход с подтяжкой
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // вход с подтяжкой
    GPIOA->ODR |= (1 << BTN_UP_PIN); // подтяжка к VCC

    // PC1 — вход с подтяжкой
    GPIOC->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOC->CRL |= GPIO_CRL_CNF1_1;
    GPIOC->ODR |= (1 << BTN_DOWN_PIN); // подтяжка к VCC
}

// Чтение кнопки (активный уровень — низкий)
uint8_t button_pressed(GPIO_TypeDef *port, uint8_t pin) {
    return !(port->IDR & (1 << pin));
}

int main(void) {
    gpio_init();

    uint8_t prev_up = 0;
    uint8_t prev_down = 0;

    while (1) {
        uint8_t up = button_pressed(GPIOA, BTN_UP_PIN);
        uint8_t down = button_pressed(GPIOC, BTN_DOWN_PIN);

        // Если нажата кнопка A (увеличение частоты)
        if (up && !prev_up) {
            if (freq < FREQ_MAX) {
                freq *= 2.0f;
                if (freq > FREQ_MAX) freq = FREQ_MAX;
                delay_ms = (uint32_t)(500.0f / freq);
            }
        }

        // Если нажата кнопка B (уменьшение частоты)
        if (down && !prev_down) {
            if (freq > FREQ_MIN) {
                freq /= 2.0f;
                if (freq < FREQ_MIN) freq = FREQ_MIN;
                delay_ms = (uint32_t)(500.0f / freq);
            }
        }

        prev_up = up;
        prev_down = down;

        // Мигание светодиода
        GPIOC->ODR ^= (1 << LED_PIN);
        delay_ms_func(delay_ms);
    }
}