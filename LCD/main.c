#include "i2c.h"
#include "lcd.h"
int main() {
    configureI2C();
    printf("Did the above command run?\n");
    int mainInit = initLCD();
    if (mainInit) {
        printf("LCD has been configured\n");
    }
    //i2c_send_byte(0x08);

}