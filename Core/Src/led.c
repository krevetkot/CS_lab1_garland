#include "led.h"
#include "pin.h"

/* Подключение светодиодов — единственное место, где упоминаются ножки */
static const Pin led_pins[LED_COUNT] = {
    [LED_GREEN]  = { GPIOD, 13 },
    [LED_YELLOW] = { GPIOD, 14 },
    [LED_RED]    = { GPIOD, 15 },
};

static bool ledIsValid(uint8_t led)
{
    return led < LED_COUNT;
}

void led_init(void)
{
    for (int i = 0; i < LED_COUNT; i++) {
        pin_init_output(&led_pins[i], false);   /* сразу «0» — светодиод не мигнёт при старте */
    }
}

void led_set(Led led, bool on)
{
    if (!ledIsValid(led)) return;
    pin_write(&led_pins[led], on);   /* «1» — горит */
}

void led_toggle(Led led)
{
    if (!ledIsValid(led)) return;
    pin_toggle(&led_pins[led]);
}

void led_set_mask(uint8_t mask)
{
    for (int i = 0; i < LED_COUNT; i++) {
        led_set((Led)i, (mask & LED_BIT(i)) != 0);
    }
}
