
#include "../hwCommon/GPIO.h"
#include "../devices/Button.h"
#include "../WaterBuddy_pinDefines.h"

#include <stdio.h>

int main()
{
    Button_init(DISPENSE_BUTTON_PIN, DISPENSE_BUTTON_GPIO);

    while(1)
    {
        printf("%d\n", Button_isPressed(DISPENSE_BUTTON_GPIO));
    }
}