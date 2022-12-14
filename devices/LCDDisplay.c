/* LCD Display Driver Code
	Allows us to display characters,
	to our LCD over I2C.
	
	Our group has added to and modified
	a pre-existing C Library for interfacing
	LCD displays for our purposes.

	Source: https://github.com/Ckath/lcd2004_i2c
*/

#include <fcntl.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <time.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/I2C.h"
#include "LCDDisplay.h"

#define LCD_ROW0 0x80
#define LCD_ROW1 0xC0
#define LCD_ROW2 0x94
#define LCD_ROW3 0xD4
#define LCD_ENABLE_BIT 0x04

#define DISPLAY_TOGGLE_SLEEP_TIME_US 600

struct lcd {
	int fd;
	uint8_t bl;
};

static void LCDDisplay_toggle(int fd, uint8_t val)
{
	sleepForUs(DISPLAY_TOGGLE_SLEEP_TIME_US);
	I2C_write(fd, val | LCD_ENABLE_BIT);
	sleepForUs(DISPLAY_TOGGLE_SLEEP_TIME_US);
	I2C_write(fd, val & ~LCD_ENABLE_BIT);
}

LCD *LCDDisplay_init(int bus)
{
	I2C_initBus(bus, LCD_I2C_ADDR);
	
	/* create lcd and associate with i2c device */
	LCD *lcd = malloc(sizeof(LCD));
	lcd->bl = LCD_BACKLIGHT;
	char dev[33];
	sprintf(dev, "/dev/i2c-%d", bus); 
	if ((lcd->fd = open(dev, O_RDWR)) < 0) {
		free(lcd);
		return NULL;
	} if (ioctl(lcd->fd, I2C_SLAVE, LCD_I2C_ADDR) < 0) { 
		LCDDisplay_delete(lcd);
		return NULL;
	}

	/* required magic taken from pi libs */
	LCDDisplay_sendCMD(lcd, 0x03);
	LCDDisplay_sendCMD(lcd, 0x03);
	LCDDisplay_sendCMD(lcd, 0x03);
	LCDDisplay_sendCMD(lcd, 0x02);

	/* initialize display config */
	LCDDisplay_sendCMD(lcd, LCD_ENTRYMODESET | LCD_ENTRYLEFT);
	LCDDisplay_sendCMD(lcd, LCD_FUNCTIONSET | LCD_2LINE);
	LCDDisplay_on(lcd);
	LCDDisplay_clear(lcd);
	return lcd;
}

void LCDDisplay_delete(LCD *lcd) {
	close(lcd->fd);
	free(lcd);
}

void LCDDisplay_sendByte(LCD *lcd, uint8_t val, uint8_t mode) {
	uint8_t buf = mode | (val & 0xF0) | lcd->bl;
	I2C_write(lcd->fd, buf);
	LCDDisplay_toggle(lcd->fd, buf);
	buf = mode | ((val << 4) & 0xF0) | lcd->bl;
	I2C_write(lcd->fd, buf);
	LCDDisplay_toggle(lcd->fd, buf);
}

void LCDDisplay_backlight(LCD *lcd, uint8_t on) {
	lcd->bl = on ? LCD_BACKLIGHT : LCD_NOBACKLIGHT;
	LCDDisplay_on(lcd);
}

void LCDDisplay_write(LCD *lcd, char *data)
{
	for (uint8_t i = 0; i < strlen(data); ++i) {
		LCDDisplay_sendChar(lcd, data[i]);
	}
}

void LCDDisplay_move(LCD *lcd, int x, int y)
{
	uint8_t pos_addr = x;
	switch(y) {
		case 0:
			pos_addr += LCD_ROW0;
			break;
		case 1:
			pos_addr += LCD_ROW1;
			break;
		case 2:
			pos_addr += LCD_ROW2;
			break;
		case 3:
			pos_addr += LCD_ROW3;
			break;
	}
	LCDDisplay_sendCMD(lcd, pos_addr);
}

void LCDDisplay_writeLine(LCD * lcd, int line, char* data) {
	//clear line
	LCDDisplay_mvWrite(lcd, 0, line, "                    ");
	LCDDisplay_mvWrite(lcd, 0, line, data);
}