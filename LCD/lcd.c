#include "i2c.h"
#include "General.h"

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100
#define Rw 0b00000010
#define Rs 0b00000001 

#define LCD_CMD  0x00
#define LCD_DATA 0x01

void LCD_strobe(unsigned char cmd) {
	i2c_send_byte((cmd | En | LCD_BACKLIGHT));
	sleepForMs(10);
	i2c_send_byte(((cmd & ~En) | LCD_BACKLIGHT));
	sleepForMs(10);
}

void LCD_sendCMD(unsigned char cmd) {
    i2c_send_byte((cmd & 0xF0) | LCD_BACKLIGHT);
    i2c_send_byte(((cmd << 4) & 0xF0) | LCD_BACKLIGHT);
    LCD_strobe(cmd);
}

void LCD_toggle() {
    i2c_send_byte(0x00);
    sleepForMs(100);
    i2c_send_byte(0x08);
    sleepForMs(100);
    i2c_send_byte(0x00);
    sleepForMs(100);
    i2c_send_byte(0x08);
    sleepForMs(100);
}

void LCD_sendDATA(unsigned char data) {
    i2c_send_byte(Rs | (data & 0xF0) | LCD_BACKLIGHT);
    i2c_send_byte(Rs | ((data << 4) & 0xF0) | LCD_BACKLIGHT);
    LCD_strobe(data);
}

int initLCD() {
    LCD_sendCMD(LCD_FUNCTIONSET | LCD_2LINE | LCD_5x8DOTS | LCD_4BITMODE);
    LCD_sendCMD(LCD_DISPLAYCONTROL | LCD_DISPLAYON);
    LCD_sendCMD(LCD_CLEARDISPLAY);
    LCD_sendCMD(LCD_ENTRYMODESET | LCD_ENTRYLEFT);
    return 1;
}