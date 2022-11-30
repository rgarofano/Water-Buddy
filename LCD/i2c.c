#include "i2c.h"
#include "General.h"

#define I2C_BUS "/dev/i2c-2"
#define I2C_ADDR 0x27         

int lcd_backlight;
int debug;
char address; 
int i2cFile;

void configureI2C() {
	runCommand("config-pin p9.19 i2c");
	runCommand("config-pin p9.20 i2c");
}

void i2c_start() {
   if((i2cFile = open(I2C_BUS, O_RDWR)) < 0) {
      printf("Error failed to open I2C bus [%s].\n", I2C_BUS);
      exit(-1);
   }
   // set the I2C slave address for all subsequent I2C device transfers
   if (ioctl(i2cFile, I2C_SLAVE, I2C_ADDR) < 0) {
      printf("Error failed to set I2C address [%d].\n", I2C_ADDR);
      exit(-1);
   }
}

void i2c_stop() { close(i2cFile); }

//Base I2C Send Command
void i2c_send_byte(unsigned char data) {
   unsigned char byte[1];
   byte[0] = data;
   write(i2cFile, byte, sizeof(byte)); 
   /* -------------------------------------------------------------------- *
    * Below wait creates 1msec delay, needed by display to catch commands  *
    * -------------------------------------------------------------------- */
   sleepForMs(1000);
}