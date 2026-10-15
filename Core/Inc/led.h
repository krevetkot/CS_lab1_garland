#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include <stdint.h>

/* Светодиоды стенда (боковая панель) */
typedef enum {
    LED_GREEN = 0,   /* одиночный зелёный */
    LED_YELLOW,      /* двухцветный, жёлтая половина */
    LED_RED,         /* двухцветный, красная половина */
    LED_COUNT        /* количество светодиодов, не светодиод */
} Led;

/* Бит светодиода в маске: LED_BIT(LED_RED) == 0b100 */
#define LED_BIT(led)  (1u << (led))

void led_set(Led led, bool on);   /* включить/выключить один светодиод */
void led_toggle(Led led);         /* переключить один светодиод */
void led_set_mask(uint8_t mask);  /* выставить все сразу: бит 1 — горит */

#endif /* LED_H */
