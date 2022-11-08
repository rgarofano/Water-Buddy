
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include "SystemTools.h"
#include "HardwareIO.h"
#include "SPI.h"

#include "MFRC522.h"

#define REG_READ_OP         1
#define REG_WRITE_OP        0
#define REG_OP_BIT          7
#define REG_READ_OP_MASK    (REG_READ_OP  << REG_OP_BIT)
#define REG_WRITE_OP_MASK   (REG_WRITE_OP << REG_OP_BIT)

#define REG_ADDR_BIT        1

static int      spiBusNum;
static int      spiChipSelect;
static char*    rstPin;
static int      rstGpioNum;

static int spiFileDesc = -1;


// Pin Defines
#define RFID_SPI_NUM 1

#define RFID_RST_PIN "p9_11"
#define RFID_RST_GPIO_NUM 30

void RFID_init(
    int     init_spiBusNum,
    int     init_spiChipSelect,
    char*   init_rstPin,
    int     init_rstGpioNum
)
{
    spiBusNum       = init_spiBusNum;
    spiChipSelect   = init_spiChipSelect;
    rstPin          = init_rstPin;
    rstGpioNum      = init_rstGpioNum;


    configPinForGPIO(rstPin, rstGpioNum, GPIO_OUT);
    gpioWrite(rstGpioNum, 1);

    spiFileDesc = SPI_initPort(
        spiBusNum,
        0,
        SPI_MODE_DEFAULT,
        BITS_PER_WORD_DEFAULT,
        SPEED_HZ_DEFAULT);
}

void RFID_writeReg(uint8_t regAddr, uint8_t regData)
{
    #define NUM_BYTES 2

    uint8_t sendBuf[NUM_BYTES];
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_WRITE_OP_MASK | (regAddr << REG_ADDR_BIT));
    // Data byte
    sendBuf[1] = regData;

	uint8_t recvBuf[NUM_BYTES] = {0};

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, NUM_BYTES);
}

uint8_t RFID_readReg(uint8_t regAddr)
{
    #define NUM_BYTES 2

    uint8_t sendBuf[NUM_BYTES];
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_READ_OP_MASK | (regAddr << REG_ADDR_BIT));
    // Second address byte (set 0 to stop reading) 
    sendBuf[1] = 0x00;

	uint8_t recvBuf[NUM_BYTES] = {0};

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, NUM_BYTES);

    // First received byte will be blank
    return recvBuf[1];
}

void RFID_writeFIFO(uint8_t *writeBuffer, uint8_t bufferSize)
{
    uint8_t transferSize = bufferSize + 1; // increment to include initial address byte
    uint8_t *sendBuf = malloc(transferSize);
    uint8_t *recvBuf = malloc(transferSize);
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_WRITE_OP_MASK | (FIFODataReg << REG_ADDR_BIT));
    for(int i = 0; i < bufferSize; i++) {
        sendBuf[i + 1] = writeBuffer[i];
    }

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, transferSize);
}

void RFID_readFIFO(uint8_t *readBuffer, uint8_t bufferSize)
{
    uint8_t transferSize = bufferSize + 1; // increment to include initial address byte
    uint8_t *sendBuf = malloc(transferSize);
    uint8_t *recvBuf = malloc(transferSize);
    // Address byte (See section 8.1.2.3 of Doc)
    for(int i = 0; i < transferSize - 1; i++) {
        sendBuf[i] = (REG_READ_OP_MASK | (FIFODataReg << REG_ADDR_BIT));
    }
    sendBuf[transferSize - 1] = 0;

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, transferSize);

    for(int i = 0; i < bufferSize; i++) {
        readBuffer[i] = recvBuf[i + 1];
    }
}

void RFID_test(void)
{
    RFID_init(1, 0, "p9_11", 30);

    int length = 6;

    uint8_t *writeData = malloc(length);

    for(int i = 0; i < length; i++) {
        writeData[i] = i * 4;
    }

    RFID_writeFIFO(writeData, (uint8_t)length);

    uint8_t *readData = malloc(length);

    RFID_readFIFO(readData, (uint8_t)length);

    for(int i = 0; i < length; i++) {
        printf("%d ", readData[i]);
    }
    printf("\n");
}