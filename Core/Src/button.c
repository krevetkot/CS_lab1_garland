#include "button.h"
#include "pin.h"
#include "main.h"   /* только ради HAL_GetTick() — системного времени (SysTick) */

/* Подключение кнопки — единственное место, где упоминается ножка */
static const Pin button_pin = { GPIOC, 15 };
#define BUTTON_ACTIVE_LEVEL  false   /* нажата = «0»: на плате подтяжка R11 к питанию */

static bool     stable_pressed   = false;  /* состояние после фильтрации */
static bool     last_raw         = false;  /* последнее прочитанное «сырое» значение */
static uint32_t last_change_time = 0;      /* когда «сырое» значение менялось в последний раз, мс */
static bool     press_event      = false;  /* было нажатие, которое программа ещё не забрала */

void button_init(void)
{
    pin_init_input(&button_pin, PIN_PULL_NONE);   /* внутренняя подтяжка не нужна: есть R11 */
}

static bool read_raw(void)
{
    return pin_read(&button_pin) == BUTTON_ACTIVE_LEVEL;
}

void button_update(void)
{
    uint32_t now = HAL_GetTick();
    bool raw = read_raw();

    if (raw != last_raw) {
        /* Сигнал изменился: возможно, это дребезг. Начинаем отсчёт заново. */
        last_raw = raw;
        last_change_time = now;
        return;
    }

    if (raw != stable_pressed && (now - last_change_time) >= BUTTON_DEBOUNCE_MS) {
        /* Сигнал продержался без изменений достаточно долго — принимаем его */
        stable_pressed = raw;
        if (stable_pressed) {
            press_event = true;   /* переход «отпущена → нажата» */
        }
    }
}

bool button_is_pressed(void)
{
    return stable_pressed;
}

bool button_was_pressed(void)
{
    bool event = press_event;
    press_event = false;
    return event;
}
