#include <stdio.h>
#include "../devices/LCDDisplay.h"

int main(int argc, char *argv[])
{
	LCD *lcd = LCDDisplay_init(2, 0x27);
	if (!lcd) {
		fputs("failed to open lcd", stderr);
		return 1;
	}

	LCDDisplay_writeLine(lcd, 0, "Water Buddy");
	LCDDisplay_writeLine(lcd, 0, "Air Buddy");
	LCDDisplay_writeLine(lcd, 0, "Earth Buddy");
	LCDDisplay_writeLine(lcd, 0, "Rock Buddy");


	return 0;
}