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
void configureI2C();

void i2c_start();

void i2c_stop();

void i2c_send_byte(unsigned char data);

void lcdputchar(char displaychar);