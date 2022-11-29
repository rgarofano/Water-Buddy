#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <stdbool.h>

#define I2CDRV_LINUX_BUS1 "/dev/i2c-2"
#define I2C_DEVICE_ADDRESS 0x27

long long getTimeInMs(void);

void sleepForMs(long long delayInMs);

void runCommand(char* command);

void configureI2C();

int initI2cBus(char* bus, int address);

void writeI2c(int i2cFileDesc, unsigned char cmd, int mode);

void lcdStrobe(int i2cFileDesc, unsigned char cmd);

void writeCMD(int i2cFileDesc, unsigned char regAddr);

int initLCD();

void lcd_display_string(int i2cFileDesc, char* string, int line);