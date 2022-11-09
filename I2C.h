
#ifndef I2C_H
#define I2C_H

#include <stdint.h>

typedef struct {
    char *scl;
    char *sda;
} i2c_port_t;

static const i2c_port_t I2C_PORTS[] = {
    { // I2C0
        .scl = "p9.17",
        .sda = "p9.18"
    }
};

// Returns I2C File Descriptor of I2C bus
// I2C bus number is in the range 1 to 3
// Configures pins on the specified bus number for I2C
int I2C_initBus(int bus, int addr);

void I2C_writeReg(int i2cFileDesc, uint8_t addr, uint8_t value);

uint8_t I2C_readReg(int i2cFileDesc, uint8_t addr);


#endif