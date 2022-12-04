
#ifndef I2C_H
#define I2C_H

/**
 * I2C Module
 * Provides an API for using the Beaglebone's I2C Ports.
 * Allows for easy port initialization and register writing/reading.
 */

#include <stdint.h>

typedef struct {
    char *scl;
    char *sda;
} i2c_port_t;

// TODO: Add other I2C ports, or change the way we init ports since some ports have more than one sda pin etc
static const i2c_port_t I2C_PORTS[] = {
    { // I2C0
        .scl = "p9.17",
        .sda = "p9.18"
    }
};

// Returns I2C File Descriptor of I2C bus
// I2C bus number must be in the range 1 to 3
// Configures pins on the specified bus number for I2C
int I2C_initBus(int bus, int addr);

void I2C_writeReg(int i2cFileDesc, uint8_t addr, uint8_t value);

uint8_t I2C_readReg(int i2cFileDesc, uint8_t addr);

void I2C_write(int fd, uint8_t b);

#endif