#include "pin.h"

#if PIN_USE_OWN_DRIVER   /* весь файл собирается, только если выбран свой драйвер */

#include "gpio_drv.h"

void pin_init_output(const Pin *pin, bool initial_level)
{
    const GpioDrvConfig config = {
        .mode          = GPIO_DRV_MODE_OUTPUT,
        .output_type   = GPIO_DRV_PUSH_PULL,
        .speed         = GPIO_DRV_SPEED_LOW,
        .pull          = GPIO_DRV_PULL_NONE,
        .initial_level = initial_level,
    };
    gpio_drv_enable_clock(pin->port);
    gpio_drv_init(pin->port, GPIO_DRV_PIN(pin->number), &config);
}

void pin_init_input(const Pin *pin, PinPull pull)
{
    static const GpioDrvPull pulls[] = {
        [PIN_PULL_NONE] = GPIO_DRV_PULL_NONE,
        [PIN_PULL_UP]   = GPIO_DRV_PULL_UP,
        [PIN_PULL_DOWN] = GPIO_DRV_PULL_DOWN,
    };
    const GpioDrvConfig config = {
        .mode = GPIO_DRV_MODE_INPUT,
        .pull = pulls[pull],
    };
    gpio_drv_enable_clock(pin->port);
    gpio_drv_init(pin->port, GPIO_DRV_PIN(pin->number), &config);
}

void pin_write(const Pin *pin, bool level)
{
    gpio_drv_write(pin->port, GPIO_DRV_PIN(pin->number), level);
}

void pin_toggle(const Pin *pin)
{
    gpio_drv_toggle(pin->port, GPIO_DRV_PIN(pin->number));
}

bool pin_read(const Pin *pin)
{
    return gpio_drv_read_pin(pin->port, GPIO_DRV_PIN(pin->number));
}

#endif /* PIN_USE_OWN_DRIVER */
