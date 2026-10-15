#include "garland.h"
#include "led.h"
#include "main.h"

#define G    LED_BIT(LED_GREEN)
#define Y    LED_BIT(LED_YELLOW)
#define R    LED_BIT(LED_RED)
#define OFF  0

#define ARRAY_SIZE(a)  (sizeof(a) / sizeof((a)[0]))

/* Кадр анимации */
typedef struct {
    uint8_t  leds;
    uint16_t duration_ms;
} Frame;

/* Режим */
typedef struct {
    const Frame *frames;
    uint8_t      count;
} Mode;


static const Frame running_light[] = {      /* 1. Бегущий огонь */
    { G, 500 }, { Y, 500 }, { R, 500 },
};

static const Frame flicker[] = {            /* 2. Мерцание */
    { Y, 150 }, { G, 150 },
};

static const Frame slow_swap[] = {          /* 3. Медленная смена */
    { R, 1000 }, { G | Y, 1000 },
};

static const Frame double_flash[] = {       /* 4. Двойная вспышка */
    { G | R, 100 }, { OFF, 100 }, { G | R, 100 }, { OFF, 700 },
};

static const Mode modes[] = {
    { running_light, ARRAY_SIZE(running_light) },
    { flicker,       ARRAY_SIZE(flicker)       },
    { slow_swap,     ARRAY_SIZE(slow_swap)     },
    { double_flash,  ARRAY_SIZE(double_flash)  },
};

#define MODE_COUNT  ARRAY_SIZE(modes)

/* Прогресс каждого режима, чтобы продолжать с прерванного места */
typedef struct {
    uint8_t  frame;     /* номер текущего кадра */
    uint32_t elapsed;   /* сколько мс текущий кадр уже показывался */
} ModeProgress;

static ModeProgress progress[MODE_COUNT];
static uint8_t      current_mode = 0;
static uint32_t     frame_start  = 0;

static void show_current_frame(void)
{
    const Mode *mode = &modes[current_mode];
    led_set_mask(mode->frames[progress[current_mode].frame].leds);
}


void garland_init(void)
{
    current_mode = 0;
    frame_start  = HAL_GetTick();
    show_current_frame();
}

void garland_update(void)
{
    const Mode   *mode = &modes[current_mode];
    ModeProgress *p    = &progress[current_mode];
    uint32_t      now  = HAL_GetTick();

    if (now - frame_start >= mode->frames[p->frame].duration_ms) {
        p->frame    = (p->frame + 1) % mode->count;
        frame_start = now;
        show_current_frame();
    }
}

void garland_next_mode(void)
{
    uint32_t now = HAL_GetTick();

    progress[current_mode].elapsed = now - frame_start;

    current_mode = (current_mode + 1) % MODE_COUNT;

    frame_start = now - progress[current_mode].elapsed;
    show_current_frame();
}
