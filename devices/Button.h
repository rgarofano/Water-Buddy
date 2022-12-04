
#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#define USER_BUTTON_PIN "p8.43"
#define USER_BUTTON_GPIO_NUM 72

void Button_init(char *pin, int gpioNum);

bool Button_isPressed(int gpioNum);

#endif