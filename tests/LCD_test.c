#include <stdio.h>
#include "../devices/LCDDisplay.h"

int main(int argc, char *argv[])
{
	LCD *lcd = lcd_init(2, 0x27);
	if (!lcd) {
		fputs("failed to open lcd", stderr);
		return 1;
	}

	lcd_mvwrite(lcd, 0, 0, "Water Buddy");
	lcd_mvwrite(lcd, 0, 1, "Hello");
	lcd_mvwrite(lcd, 0, 3, "Testing");
	return 0;
}