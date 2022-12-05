#include "../devices/LCDDisplay.h"

#include "WaterBuddy_pinDefines.h"

#define  FIRST_LINE 0
#define SECOND_LINE 1
#define  THIRD_LINE 2
#define FOURTH_LINE 3

static LCD* lcd;

void DisplayText_init(void) {
    lcd = LCDDisplay_init(LCD_I2C_BUS, LCD_I2C_ADDR);
}

void DisplayText_idleMessage(void)
{
    LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "   Water Buddy :)   ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, "its time to drink up")
}