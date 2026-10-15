#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#ifndef BUTTON_DEBOUNCE_MS
#define BUTTON_DEBOUNCE_MS  30
#endif

void button_update(void);       /* опрос кнопки: вызывать как можно чаще из главного цикла */
bool button_is_pressed(void);   /* текущее состояние (уже без дребезга) */
bool button_was_pressed(void);  /* было ли новое нажатие; после вызова флаг сбрасывается */
void button_init(void);         /* настроить ножку кнопки */

#endif /* BUTTON_H */
