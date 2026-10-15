#include "pin.h"

#if !PIN_USE_OWN_DRIVER   /* весь файл собирается, только если выбран HAL */

#include "main.h"   /* HAL */

static void enable_clock(GPIO_TypeDef *port)
{
    if      (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
}

static uint16_t mask(const Pin *pin)
{
    return (uint16_t)(1u << pin->number);
}

void pin_init_output(const Pin *pin, bool initial_level)
{
    GPIO_InitTypeDef init = {0};
    enable_clock(pin->port);
    /* В HAL уровень нужно выставить заранее: HAL_GPIO_Init его не задаёт */
    HAL_GPIO_WritePin(pin->port, mask(pin), initial_level ? GPIO_PIN_SET : GPIO_PIN_RESET);
    init.Pin   = mask(pin);
    init.Mode  = GPIO_MODE_OUTPUT_PP;
    init.Pull  = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(pin->port, &init);
}

void pin_init_input(const Pin *pin, PinPull pull)
{
    static const uint32_t pulls[] = {
        [PIN_PULL_NONE] = GPIO_NOPULL,
        [PIN_PULL_UP]   = GPIO_PULLUP,
        [PIN_PULL_DOWN] = GPIO_PULLDOWN,
    };
    GPIO_InitTypeDef init = {0};
    enable_clock(pin->port);
    init.Pin  = mask(pin);
    init.Mode = GPIO_MODE_INPUT;
    init.Pull = pulls[pull];
    HAL_GPIO_Init(pin->port, &init);
}

void pin_write(const Pin *pin, bool level)
{
    HAL_GPIO_WritePin(pin->port, mask(pin), level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void pin_toggle(const Pin *pin)
{
    HAL_GPIO_TogglePin(pin->port, mask(pin));
}

bool pin_read(const Pin *pin)
{
    return HAL_GPIO_ReadPin(pin->port, mask(pin)) == GPIO_PIN_SET;
}

#endif /* !PIN_USE_OWN_DRIVER */
