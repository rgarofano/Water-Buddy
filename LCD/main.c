#include "i2c.h"

int main() {
    configureI2C();
    printf("Pins have been configured for I2C\n");
    int mainInit = initLCD();
    lcd_display_string(mainInit, "hello", 1);
}