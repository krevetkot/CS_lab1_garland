#include "button.h"
#include "main.h"

#define BUTTON_PORT          GPIOC
#define BUTTON_PIN           GPIO_PIN_15
#define BUTTON_ACTIVE_LEVEL  GPIO_PIN_RESET   /* 0 - нажата */

static bool     stable_pressed   = false;  /* состояние после фильтрации */
static bool     last_raw         = false;  /* последнее прочитанное «сырое» значение */
static uint32_t last_change_time = 0;      /* когда «сырое» значение менялось в последний раз, мс */
static bool     press_event      = false;  /* было нажатие, которое программа ещё не забрала */

static bool read_raw(void)
{
    return HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == BUTTON_ACTIVE_LEVEL;
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
