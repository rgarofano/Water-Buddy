
#ifndef HARDWAREIO_H
#define HARDWAREIO_H

#include <stdint.h>

/**
 * This header file includes general functions for:
 * Byte Operations
 * Configuring Pins
 * GPIO
 * Beaglebone's LEDs
 * Analog IO
 * I2C
 */



// Bit Operations
#define BITS_PER_BYTE 8

// Returns the byte rotated to the right by n bits
char rotateCharRight(char byte, int n);

// Returns the byte rotated to the left by n bits
char rotateCharLeft(char bbyte, int n);



// Pin Config
void configPin(const char* pin, const char* config);



// GPIO
#define GPIO_IN "in"
#define GPIO_OUT "out"

// A more specific version of configPin(pin, config)
void configPinForGPIO(const char* pin, const int gpioNum, const char *direction);

int gpioRead(int gpioNum);

void gpioWrite(int gpioNum, int value);



// LED
#define LED_ON "1"
#define LED_OFF "0"

static const char *LED_TRIGGER_DEFAULT[] = {"heartbeat", "mmc0", "cpu0", "mmc1"};

// LED number is in the range 0 to 3
void setLEDAttribute(const int LEDNum, const char* attribute, const char* value);



// Analog IO
// Analog Channels are 0-indexed
#define NUM_ANALOG_CHANNELS 7
#define MAX_A2D_VALUE 4095.0f // This is a float so that float division can be done without casting
#define MAX_AIN_VOLTAGE 1.8f

// Returns the A2D value read from the specified analog channel, which is an integer in the range 0 to MAX_A2D_VALUE
// Analog channels are in the range 0 to 6
int analogRead(int analogNum);

// Returns the voltage read from the specified analog channel, which is a floating point number in the range 0 to MAX_AIN_VOLTAGE Volts
// Analog channels are in the range 0 to 6
float analogReadVoltage(int analogChannel);



// I2C
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



// SPI
#define NUM_SPI_BUSES 2
#define SPI_MODE_DEFAULT 0
#define BITS_PER_WORD_DEFAULT 8
#define SPEED_HZ_DEFAULT 500000

typedef struct {
    char *cs0;
    char *cs1;
    char *d0; // AKA miso
    char *d1; // AKA mosi
    char *sclk;
} spi_port_t;

static const spi_port_t SPI_PORTS[] = {
    { // SPI0
        .cs0    = "p9_17",
        .cs1    = "none",
        .d0     = "p9_21",
        .d1     = "p9_18",
        .sclk   = "p9_22"
    },
    { // SPI1
        .cs0    = "p9_28",
        .cs1    = "p9_42",
        .d0     = "p9_29",
        .d1     = "p9_30",
        .sclk   = "p9_31"
    }
};

// Configures pins associated with the specified port number for spi
// Opens the file associated with that port and the specified chipSelect number
// Returns the file descriptor
int SPI_initPort(
    int port,
    int chipSelect,
    int spiMode,
    int bitsPerWord,
    int speedHz
);

// Returns the status
// Status >= 0 is success, < 0 is error
int SPI_transfer(int spiFileDesc, uint8_t *send, uint8_t *recv, int numBytes);

#endif