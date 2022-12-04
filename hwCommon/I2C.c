
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "SystemTools.h"

#include "I2C.h"

static const char *I2CDRV_LINUX_BUS[] = {
    "/dev/i2c-0",
    "/dev/i2c-1",
    "/dev/i2c-2"
};

int I2C_initBus(int bus, int addr)
{
    if(bus < 1 || bus > 2) {
        printf("I2C: Invalid port number");
        exit(1);
    }

    int i2cFileDesc = open(I2CDRV_LINUX_BUS[bus], O_RDWR);

    // Looks like this function just changes the i2c device addr, maybe to talk to a different device connected to the same port.
    // Consider breaking this function up so we aren't calling open (above) every time we want to talk to a different device
    int result = ioctl(i2cFileDesc, I2C_SLAVE, addr);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }

    configPin(I2C_PORTS[bus - 1].scl, "i2c");
    configPin(I2C_PORTS[bus - 1].sda, "i2c");

    return i2cFileDesc;
}

void I2C_writeReg(int i2cFileDesc, uint8_t addr, uint8_t value)
{
    uint8_t buff[2];

    buff[0] = addr;
    buff[1] = value;

    int res = write(i2cFileDesc, buff, 2);

    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

uint8_t I2C_readReg(int i2cFileDesc, uint8_t addr)
{
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &addr, sizeof(addr));

    if (res != sizeof(addr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }

    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));

    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }

    return value;
}

/*Simple I2C Write Function for our LCD Display
  purposes taken from a pre-existing library
  
  Source: https://github.com/Ckath/lcd2004_i2c*/
void I2C_write(int fd, uint8_t b)
{
	write(fd, &b, 1);
}