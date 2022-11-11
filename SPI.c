
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "SystemTools.h"

#include "SPI.h"

// The code for setting port parameters was taken and modified from spidev_test.c here:
// https://github.com/derekmolloy/exploringBB/blob/version2/chp08/spi/spidev_test/spidev_test.c
int SPI_initPort(int port, int chipSelect, int spiMode, int bitsPerWord, int speedHz)
{
    // SPI0 has 1 peripheral slot {0}
    // SPI1 has 2 peripheral slots {0, 1}
    switch(port) {
        case 0:
            if(chipSelect != 0) {
                printf("SPI: Invalid chip select number");
                exit(1);
            }
            break;
        case 1: 
            if(chipSelect < 0 || chipSelect > 1) {
                printf("SPI: Invalid chip select number");
                exit(1);
            }
            break;
        default:
            printf("SPI: Invalid port number");
            exit(1);
    }

    configPin(SPI_PORTS[port].cs0,  "spi_cs");
    if(!strcmp(SPI_PORTS[port].cs1, "none")) {
        configPin(SPI_PORTS[port].cs1,  "spi_cs");
    }
    configPin(SPI_PORTS[port].d0,   "spi");
    configPin(SPI_PORTS[port].d1,   "spi");
    configPin(SPI_PORTS[port].sclk, "spi_sclk");

    #define SPI_DEVICE_LENGTH 16
    char spiDevice[SPI_DEVICE_LENGTH];
    sprintf(spiDevice, "/dev/spidev%d.%d", port, chipSelect);
    spiDevice[SPI_DEVICE_LENGTH - 1] = '\0';
    
    // Open Device
    int spiFileDesc = open(spiDevice, O_RDWR);
    if (spiFileDesc < 0) {
		printf("Error: Can't open device %s", spiDevice);
        exit(1);
    }

    // Set port parameters

	// SPI mode
	int errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_MODE, &spiMode);
	if (errorCheck < 0) {
		printf("Set SPI mode failed");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_MODE, &spiMode);
	if (errorCheck < 0) {
		printf("Get SPI mode failed");
        exit(1);
    }

	// Bits per word
	errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord);
	if (errorCheck < 0) {
		printf("Set bits per word failed");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_BITS_PER_WORD, &bitsPerWord);
	if (errorCheck < 0) {
		printf("Get bits per word failed");
        exit(1);
    }

	// Max Speed (Hz)
	errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_MAX_SPEED_HZ, &speedHz);
	if (errorCheck < 0) {
		printf("Set max speed hz failed");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_MAX_SPEED_HZ, &speedHz);
	if (errorCheck < 0) {
		printf("Get max speed hz failed");
        exit(1);
    }

    return spiFileDesc;
}

int SPI_transfer(int spiFileDesc, uint8_t *send, uint8_t *recv, int numBytes)
{
	struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)send,
        .rx_buf = (unsigned long)recv,
        .len = numBytes,
        .delay_usecs = 0,
        .speed_hz = SPEED_HZ_DEFAULT,
        .bits_per_word = BITS_PER_WORD_DEFAULT
    };

	int status = ioctl(spiFileDesc, SPI_IOC_MESSAGE(1), &transfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
    }

    return status;
}

