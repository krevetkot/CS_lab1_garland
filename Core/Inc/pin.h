#ifndef PIN_H
#define PIN_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"   /* GPIO_TypeDef, GPIOA..GPIOK */

/* Какую реализацию собирать:
   1 — собственный драйвер gpio_drv (файл pin_drv.c)
   0 — библиотека HAL           (файл pin_hal.c) */
#ifndef PIN_USE_OWN_DRIVER
#define PIN_USE_OWN_DRIVER  1
#endif

/* Ножка микроконтроллера: порт и номер 0..15 */
typedef struct {
    GPIO_TypeDef *port;
    uint8_t       number;
} Pin;

typedef enum {
    PIN_PULL_NONE,
    PIN_PULL_UP,
    PIN_PULL_DOWN,
} PinPull;

void pin_init_output(const Pin *pin, bool initial_level);   /* выход push-pull; уровень ставится ДО включения */
void pin_init_input(const Pin *pin, PinPull pull);
void pin_write(const Pin *pin, bool level);
void pin_toggle(const Pin *pin);
bool pin_read(const Pin *pin);

#endif /* PIN_H */
