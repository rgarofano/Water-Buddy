
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#include "SystemTools.h"

#include "HardwareIO.h"



// Byte Operations
char rotateCharRight(char byte, int n)
{
    n = n % BITS_PER_BYTE;
    return (byte >> n) | (byte << (BITS_PER_BYTE - n));
}

char rotateCharLeft(char byte, int n)
{
    return rotateCharRight(byte, BITS_PER_BYTE - n);
}



// Pin Config
void configPin(const char* pin, const char* config)
{
    const int COMMAND_MAX_SIZE = 64;
    char configPinCommand[COMMAND_MAX_SIZE];
    sprintf(configPinCommand, "config-pin %s %s > /dev/null", pin, config);
    configPinCommand[COMMAND_MAX_SIZE - 1] = 0;
    system(configPinCommand);
}



// GPIO
#define SLEEP_TIME_AFTER_EXPORT_MS 1000
static void exportGpioNum(const int gpioNum)
{
    FILE *gpioExportFile = fopenWithCheck("/sys/class/gpio/export", "w");
    fprintf(gpioExportFile, "%d", gpioNum);
    fclose(gpioExportFile);

    // Must sleep for a short time while the necessary directories for the specified gpioNum are created
    sleepForMs(SLEEP_TIME_AFTER_EXPORT_MS);
}

void configPinForGPIO(const char* pin, const int gpioNum, const char* gpioDirection)
{
    configPin(pin, "gpio");

    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioDirectionFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioDirectionFileName, "/sys/class/gpio/gpio%d/direction", gpioNum);

    FILE *gpioDirectionFile = fopenWithCheck(gpioDirectionFileName, "w");

    fprintf(gpioDirectionFile, gpioDirection);

    fclose(gpioDirectionFile);
}

int gpioRead(int gpioNum)
{
    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioValueFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioValueFileName, "/sys/class/gpio/gpio%d/value", gpioNum);

    FILE* gpioValueFile = fopenWithCheck(gpioValueFileName, "r");

    const int VAL_BUFFER_MAX_SIZE = 16;
    char valueBuffer[VAL_BUFFER_MAX_SIZE];
    fgets(valueBuffer, VAL_BUFFER_MAX_SIZE, gpioValueFile);

    fclose(gpioValueFile);

    int value = atoi(valueBuffer);

    return value;
}

void gpioWrite(int gpioNum, int value)
{
    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioValueFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioValueFileName, "/sys/class/gpio/gpio%d/value", gpioNum);

    FILE* gpioValueFile = fopenWithCheck(gpioValueFileName, "w");

    fprintf(gpioValueFile, "%d", value);

    fclose(gpioValueFile);
}



// LED
void setLEDAttribute(const int LEDNum, const char* attribute, const char* value)
{
    if(attribute == NULL || value == NULL) {
        printf("ERROR: NULL arguments in setLEDAttribute() for LED%d", LEDNum);
        exit(1);
    }

    const int LED_FILE_NAME_MAX_SIZE = 50;
    char ledAttFileName[LED_FILE_NAME_MAX_SIZE];
    sprintf(ledAttFileName, "/sys/class/leds/beaglebone:green:usr%d/%s", LEDNum, attribute);

    FILE* ledAttFile = fopenWithCheck(ledAttFileName, "w");

    fprintf(ledAttFile, "%s", value);

    fclose(ledAttFile);
}



// Analog IO
int analogRead(int analogChannel)
{
    const int ANALOG_FILE_NAME_MAX_SIZE = 64;
    char analogValueFileName[ANALOG_FILE_NAME_MAX_SIZE];
    sprintf(analogValueFileName, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", analogChannel);

    FILE* analogValueFile = fopenWithCheck(analogValueFileName, "r");

    int a2DReading = 0;
    int itemsRead = fscanf(analogValueFile, "%d", &a2DReading);
    if(itemsRead <= 0) {
        printf("Error: Unable to read values from voltage input file.\n");
        exit(1);
    }

    fclose(analogValueFile);

    return a2DReading;
}

float analogReadVoltage(int analogChannel)
{
    float a2dValue = (float)analogRead(analogChannel);
    return ((a2dValue / MAX_A2D_VALUE) * MAX_AIN_VOLTAGE);
}



// I2C
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

void I2C_writeReg(int i2cFileDesc, unsigned char addr, unsigned char value)
{
    unsigned char buff[2];

    buff[0] = addr;
    buff[1] = value;

    int res = write(i2cFileDesc, buff, 2);

    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

unsigned char I2C_readReg(int i2cFileDesc, unsigned char addr)
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



// SPI
int SPI_initPort(int port, int chipSelect)
{
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
    if(port != 0) {
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
    int errorCheck = 0;
    int spiMode = 0;
    int bitsPerWord = 8;
    int speedHz = 500000;

	// SPI mode
	errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_MODE, &spiMode);
	if (errorCheck < 0) {
		printf("can't set spi mode");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_MODE, &spiMode);
	if (errorCheck < 0) {
		printf("can't get spi mode");
        exit(1);
    }

	// Bits per word
	errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord);
	if (errorCheck < 0) {
		printf("can't set bits per word");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_BITS_PER_WORD, &bitsPerWord);
	if (errorCheck < 0) {
		printf("can't get bits per word");
        exit(1);
    }

	// Max Speed (Hz)
	errorCheck = ioctl(spiFileDesc, SPI_IOC_WR_MAX_SPEED_HZ, &speedHz);
	if (errorCheck < 0) {
		printf("can't set max speed hz");
        exit(1);
    }
	errorCheck = ioctl(spiFileDesc, SPI_IOC_RD_MAX_SPEED_HZ, &speedHz);
	if (errorCheck < 0) {
		printf("can't get max speed hz");
        exit(1);
    }

	// printf("spi mode: %d\n", mode);
	// printf("bits per word: %d\n", bitsPerWord);
	// printf("max speed: %d Hz (%d KHz)\n", speedHz, speedHz/1000);

    return spiFileDesc;
}

