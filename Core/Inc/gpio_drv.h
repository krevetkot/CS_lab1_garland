#ifndef GPIO_DRV_H
#define GPIO_DRV_H

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"   /* CMSIS: GPIO_TypeDef, адреса портов, биты регистров RCC */

/* Маска ножки: GPIO_DRV_PIN(13) == 0x2000. Можно объединять через | */
#define GPIO_DRV_PIN(n)  ((uint16_t)(1u << (n)))

/* Значения совпадают с кодами полей регистров (RM0090, GPIO registers) */
typedef enum {
    GPIO_DRV_MODE_INPUT  = 0,   /* 00 — вход */
    GPIO_DRV_MODE_OUTPUT = 1,   /* 01 — выход */
    GPIO_DRV_MODE_ALT    = 2,   /* 10 — альтернативная функция */
    GPIO_DRV_MODE_ANALOG = 3,   /* 11 — аналоговый */
} GpioDrvMode;

typedef enum {
    GPIO_DRV_PUSH_PULL  = 0,
    GPIO_DRV_OPEN_DRAIN = 1,
} GpioDrvOutputType;

typedef enum {
    GPIO_DRV_SPEED_LOW       = 0,
    GPIO_DRV_SPEED_MEDIUM    = 1,
    GPIO_DRV_SPEED_HIGH      = 2,
    GPIO_DRV_SPEED_VERY_HIGH = 3,
} GpioDrvSpeed;

typedef enum {
    GPIO_DRV_PULL_NONE = 0,
    GPIO_DRV_PULL_UP   = 1,
    GPIO_DRV_PULL_DOWN = 2,
} GpioDrvPull;

typedef struct {
    GpioDrvMode       mode;
    GpioDrvOutputType output_type;    /* для OUTPUT и ALT */
    GpioDrvSpeed      speed;          /* для OUTPUT и ALT */
    GpioDrvPull       pull;
    bool              initial_level;  /* для OUTPUT: уровень, выставляемый ДО включения выхода */
    uint8_t           alt_function;   /* для ALT: номер функции 0..15 */
} GpioDrvConfig;

/* Включить тактирование порта (без него регистры порта не работают) */
void gpio_drv_enable_clock(GPIO_TypeDef *port);

/* Настроить одну или несколько ножек порта (pins — маска). Можно вызывать повторно. */
void gpio_drv_init(GPIO_TypeDef *port, uint16_t pins, const GpioDrvConfig *config);

/* Прочитать текущие настройки одной ножки (pin_number — номер 0..15) */
void gpio_drv_get_config(GPIO_TypeDef *port, uint8_t pin_number, GpioDrvConfig *config);

/* Запись */
void gpio_drv_write(GPIO_TypeDef *port, uint16_t pins, bool level);               /* всем ножкам маски — один уровень */
void gpio_drv_write_masked(GPIO_TypeDef *port, uint16_t pins, uint16_t values);   /* каждой ножке — свой уровень */
void gpio_drv_toggle(GPIO_TypeDef *port, uint16_t pins);

/* Чтение */
uint16_t gpio_drv_read(GPIO_TypeDef *port, uint16_t pins);   /* уровни ножек маски (остальные биты = 0) */
bool     gpio_drv_read_pin(GPIO_TypeDef *port, uint16_t pin);

#endif /* GPIO_DRV_H */
