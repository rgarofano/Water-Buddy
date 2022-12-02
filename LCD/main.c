#include "i2c.h"
#include "lcd.h"
#include "General.h"

int main() {
    i2c_start();
    configureI2C();
    initLCD();
    i2c_send_byte(0x00);
}