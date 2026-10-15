#include "gpio_drv.h"

/* Порты идут в памяти подряд: GPIOB = GPIOA + 0x400, GPIOC = GPIOA + 0x800 и т. д. */
#define GPIO_PORT_STRIDE  0x400u

void gpio_drv_enable_clock(GPIO_TypeDef *port)
{
    uint32_t index = ((uint32_t)port - GPIOA_BASE) / GPIO_PORT_STRIDE;   /* A=0, B=1, C=2, D=3 ... */
    RCC->AHB1ENR |= (1u << index);    /* биты GPIOxEN в AHB1ENR идут подряд с бита 0 */
    (void)RCC->AHB1ENR;               /* пустое чтение — пауза, пока тактирование включится */
}

void gpio_drv_init(GPIO_TypeDef *port, uint16_t pins, const GpioDrvConfig *config)
{
    /* Собираем маски сразу для всех ножек, чтобы каждый регистр записать один раз */
    uint32_t mask2 = 0, moder = 0, ospeed = 0, pupd = 0;     /* поля по 2 бита на ножку */
    uint32_t afr_mask[2] = {0, 0}, afr[2] = {0, 0};          /* поля по 4 бита на ножку */

    for (uint32_t i = 0; i < 16; i++) {
        if ((pins & (1u << i)) == 0) continue;
        mask2  |= 3u << (2 * i);
        moder  |= (uint32_t)config->mode  << (2 * i);
        ospeed |= (uint32_t)config->speed << (2 * i);
        pupd   |= (uint32_t)config->pull  << (2 * i);
        afr_mask[i / 8] |= 0xFu << (4 * (i % 8));
        afr[i / 8]      |= (uint32_t)(config->alt_function & 0xFu) << (4 * (i % 8));
    }

    /* 1. Сначала — выходной уровень. ODR работает, даже пока ножка вход,
          поэтому в момент включения выхода уровень уже будет правильным. */
    if (config->mode == GPIO_DRV_MODE_OUTPUT) {
        gpio_drv_write(port, pins, config->initial_level);
    }

    /* 2. Тип выхода, скорость, подтяжка, альтернативная функция */
    if (config->output_type == GPIO_DRV_OPEN_DRAIN) {
        port->OTYPER |= pins;
    } else {
        port->OTYPER &= ~(uint32_t)pins;
    }
    port->OSPEEDR = (port->OSPEEDR & ~mask2) | ospeed;
    port->PUPDR   = (port->PUPDR   & ~mask2) | pupd;
    port->AFR[0]  = (port->AFR[0]  & ~afr_mask[0]) | afr[0];
    port->AFR[1]  = (port->AFR[1]  & ~afr_mask[1]) | afr[1];

    /* 3. Режим — последним: только сейчас ножка начинает работать по-новому */
    port->MODER = (port->MODER & ~mask2) | moder;
}

void gpio_drv_get_config(GPIO_TypeDef *port, uint8_t n, GpioDrvConfig *config)
{
    if (n > 15) return;
    config->mode          = (GpioDrvMode)((port->MODER >> (2 * n)) & 3u);
    config->output_type   = (GpioDrvOutputType)((port->OTYPER >> n) & 1u);
    config->speed         = (GpioDrvSpeed)((port->OSPEEDR >> (2 * n)) & 3u);
    config->pull          = (GpioDrvPull)((port->PUPDR >> (2 * n)) & 3u);
    config->initial_level = ((port->ODR >> n) & 1u) != 0;
    config->alt_function  = (uint8_t)((port->AFR[n / 8] >> (4 * (n % 8))) & 0xFu);
}

void gpio_drv_write(GPIO_TypeDef *port, uint16_t pins, bool level)
{
    /* BSRR: биты 0–15 ставят «1», биты 16–31 ставят «0». Одна атомарная запись,
       остальные ножки порта не затрагиваются. */
    port->BSRR = level ? pins : ((uint32_t)pins << 16);
}

void gpio_drv_write_masked(GPIO_TypeDef *port, uint16_t pins, uint16_t values)
{
    uint32_t set   = values & pins;               /* кому поставить «1» */
    uint32_t reset = (uint16_t)~values & pins;    /* кому поставить «0» */
    port->BSRR = set | (reset << 16);             /* всё одной записью */
}

void gpio_drv_toggle(GPIO_TypeDef *port, uint16_t pins)
{
    uint16_t current = (uint16_t)port->ODR;
    gpio_drv_write_masked(port, pins, (uint16_t)~current);   /* каждой ножке — противоположный уровень */
}

uint16_t gpio_drv_read(GPIO_TypeDef *port, uint16_t pins)
{
    return (uint16_t)(port->IDR & pins);
}

bool gpio_drv_read_pin(GPIO_TypeDef *port, uint16_t pin)
{
    return (port->IDR & pin) != 0;
}
