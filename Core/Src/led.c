#include "led.h"
#include "main.h"
#include <stdbool.h>

typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
} LedPin;

static const LedPin led_pins[LED_COUNT] = {
    [LED_GREEN]  = { GPIOD, GPIO_PIN_13 },
    [LED_YELLOW] = { GPIOD, GPIO_PIN_14 },
    [LED_RED]    = { GPIOD, GPIO_PIN_15 },
};

bool ledIsValid(uint8_t led){
	if (led >= LED_COUNT) return false;
	return true;
}

void led_set(Led led, bool on)
{
    if (!ledIsValid(led)) return;
    HAL_GPIO_WritePin(led_pins[led].port, led_pins[led].pin,
                      on ? GPIO_PIN_SET : GPIO_PIN_RESET);   /* 1 — горит */
}

void led_toggle(Led led)
{
    if (!ledIsValid(led)) return;
    HAL_GPIO_TogglePin(led_pins[led].port, led_pins[led].pin);
}

void led_set_mask(uint8_t mask)
{
    for (int i = 0; i < LED_COUNT; i++) {
        led_set((Led)i, (mask & LED_BIT(i)) != 0);
    }
}
