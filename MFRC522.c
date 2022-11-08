
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <errno.h>

#include "SystemTools.h"
#include "HardwareIO.h"
#include "SPI.h"

#include "MFRC522.h"

static int      spiBusNum;
static char*    rstPin;
static int      rstGpioNum;
static char*    sdaPin;

static int spiFileDesc = -1;

#define REG_READ_OP         1
#define REG_WRITE_OP        0
#define REG_OP_BIT          7
#define REG_READ_OP_MASK    (REG_READ_OP  << REG_OP_BIT)
#define REG_WRITE_OP_MASK   (REG_WRITE_OP << REG_OP_BIT)

#define REG_ADDR_BIT        1



// Pin Defines
#define RFID_SPI_NUM 1

#define RFID_RST_PIN "p9_11"
#define RFID_RST_GPIO_NUM 30

#define RFID_IRQ_PIN
#define RFID_IRQ_GPIO_NUM

// MISO, Master In Slave Out
#define RFID_MISO_PIN "p9_29" // SPI1_D0

// MOSI, Master Out Slave In
#define RFID_MOSI_PIN "p9_30" // SPI1_D1

// SCK, Clock
#define RFID_SCK_PIN "p9_31" // SPI1_SCLK

// SDA/SS, Slave Select
#define RFID_SDA_PIN "p9_28" // SPI1_CS0

void RFID_init(
    int     init_spiBusNum,
    char*   init_rstPin,
    int     init_rstGpioNum,
    char*   init_sdaPin
)
{
    spiBusNum   = init_spiBusNum;
    rstPin      = init_rstPin;
    rstGpioNum  = init_rstGpioNum;
    sdaPin      = init_sdaPin;


    configPinForGPIO(rstPin, rstGpioNum, GPIO_OUT);
    gpioWrite(rstGpioNum, 1);

    spiFileDesc = SPI_initPort(spiBusNum, 0);
}

unsigned char RFID_readReg(unsigned char regAddr)
{
    #define NUM_BYTES 2

    unsigned char sendBuf[NUM_BYTES];
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_READ_OP_MASK | (regAddr << REG_ADDR_BIT));
    sendBuf[1] = 0x00;

	unsigned char recvBuf[NUM_BYTES] = {0};

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, NUM_BYTES);

    // First received byte will be blank
    return recvBuf[1];
}

unsigned char RFID_writeReg(unsigned char regAddr, unsigned char regData)
{
    #define NUM_BYTES 2

    unsigned char sendBuf[NUM_BYTES];
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_READ_OP_MASK | (regAddr << REG_ADDR_BIT));
    sendBuf[1] = regData;

	unsigned char recvBuf[NUM_BYTES] = {0};

    return SPI_transfer(spiFileDesc, sendBuf, recvBuf, NUM_BYTES);
}

// From https://www.emcraft.com/stm32f769i-discovery-board/accessing-spi-devices-in-linux
static void test1(void)
{
    unsigned char data = RFID_readReg(VersionReg);

    printf("data: %02x\n", data);
}

void RFID_test(void)
{
    RFID_init(
        RFID_SPI_NUM,
        RFID_RST_PIN,
        RFID_RST_GPIO_NUM,
        RFID_SDA_PIN
    );

    test1();
}