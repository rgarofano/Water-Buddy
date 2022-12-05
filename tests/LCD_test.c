
#include <stdio.h>

#include "../devices/LCDDisplay.h"
#include "../WaterBuddy_pinDefines.h"

int main(int argc, char *argv[])
{
	LCD *lcd = LCDDisplay_init(LCD_I2C_BUS, LCD_I2C_ADDR);
	if (!lcd) {
		fputs("failed to open lcd", stderr);
		return 1;
	}

	LCDDisplay_writeLine(lcd, 0, "Water Buddy");
	LCDDisplay_writeLine(lcd, 1, "Air Buddy");
	LCDDisplay_writeLine(lcd, 2, "Earth Buddy");
	LCDDisplay_writeLine(lcd, 3, "Rock Buddy");


	return 0;
}