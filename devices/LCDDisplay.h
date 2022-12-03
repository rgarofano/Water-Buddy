#ifndef LCD_DISPLAY_
#define LCD_DISPLAY_
#include <stdint.h>

/* general commands */
#define LCD_CLEARDISPLAY 0x01
#define LCD_CURSORSHIFT 0x10
#define LCD_DISPLAYCONTROL 0x08
#define LCD_ENTRYMODESET 0x04
#define LCD_FUNCTIONSET 0x20
#define LCD_RETURNHOME 0x02
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
/* entrymode flags */
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
/* displaycontrol flags */
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00
/* cursorshift flags */
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00
/* functionset flags */
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00
/* write modes for lcd_write_byte */
#define LCD_CMD 0
#define LCD_CHR 1

typedef struct lcd LCD;

LCD *LCDDisplay_init(int bus, int addr);
void LCDDisplay_delete(LCD *lcd);
void LCDDisplay_sendByte(LCD *lcd, uint8_t val, uint8_t mode); /* <- not meant for direct calling */
void LCDDisplay_backlight(LCD *lcd, uint8_t on);
void LCDDisplay_move(LCD *lcd, int x, int y);
void LCDDisplay_write(LCD *lcd, char *data);

/* convenient wrapper macros */
#define lcd_mvwrite(lcd, x, y, data) do { LCDDisplay_move(lcd, x, y); LCDDisplay_write(lcd, data); } while (0)
#define lcd_send_cmd(lcd, cmd) LCDDisplay_sendByte(lcd, cmd, LCD_CMD)
#define lcd_send_chr(lcd, chr) LCDDisplay_sendByte(lcd, chr, LCD_CHR)
#define lcd_off(lcd) lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYOFF)
#define lcd_on(lcd) lcd_send_cmd(lcd, LCD_DISPLAYCONTROL | LCD_DISPLAYON)
#define lcd_clear(lcd) do { lcd_send_cmd(lcd, LCD_CLEARDISPLAY); lcd_send_cmd(lcd, LCD_RETURNHOME); } while (0)
#define lcd_cgramset(lcd, addr) lcd_send_cmd(lcd, LCD_SETCGRAMADDR|((addr)<<3))
#endif
