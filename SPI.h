
#ifndef SPI_H
#define SPI_H

#include <stdint.h>

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
int SPI_initPort(int port, int chipSelect, int spiMode, int bitsPerWord, int speedHz);

// Returns the status
// Status >= 0 is success, < 0 is error
int SPI_transfer(int spiFileDesc, uint8_t *send, uint8_t *recv, int numBytes);

#endif