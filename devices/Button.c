
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../hwCommon/GPIO.h"

#include "Button.h"

#define BUTTON_VALUE_PRESSED 1
#define USER_BUTTON_VALUE_PRESSED 0

void Button_init(char *pin, int gpioNum)
{
    GPIO_configPin(pin, gpioNum, GPIO_IN);

}

bool Button_isPressed(int gpioNum)
{
    if(gpioNum == USER_BUTTON_GPIO_NUM) {
        return (GPIO_read(gpioNum) == USER_BUTTON_VALUE_PRESSED);
    }
    return (GPIO_read(gpioNum) == BUTTON_VALUE_PRESSED);
}
