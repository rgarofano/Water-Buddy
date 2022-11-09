
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include "SystemTools.h"
#include "HardwareIO.h"

#include "MFRC522.h"



static int      spiBusNum;
static int      spiChipSelect;
static char*    rstPin;
static int      rstGpioNum;

static int spiFileDesc = -1;



// Pin Defines
#define RFID_SPI_NUM 1

#define RFID_RST_PIN "p9_11"
#define RFID_RST_GPIO_NUM 30

void RFID_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum)
{
    spiBusNum       = init_spiBusNum;
    spiChipSelect   = init_spiChipSelect;
    rstPin          = init_rstPin;
    rstGpioNum      = init_rstGpioNum;


    configPinForGPIO(rstPin, rstGpioNum, GPIO_OUT);
    gpioWrite(rstGpioNum, 0);
    sleepForUs(2);
    gpioWrite(rstGpioNum, 1);

    spiFileDesc = SPI_initPort(
        spiBusNum,
        0,
        SPI_MODE_DEFAULT,
        BITS_PER_WORD_DEFAULT,
        SPEED_HZ_DEFAULT
    );

    // Default Values?
    RFID_writeReg(TModeReg, 0x80);
    RFID_writeReg(TPrescalerReg, 0xA9);
    RFID_writeReg(TReloadRegH, 0x03);
    RFID_writeReg(TReloadRegL, 0xE8);
    RFID_writeReg(TxASKReg, 0x40);
    RFID_writeReg(ModeReg, 0x3D);

    // Turn antenna on
    uint8_t regVal = RFID_readReg(TxControlReg);
    RFID_writeReg(TxControlReg, regVal | 0x03);
    sleepForMs(1);
}

void RFID_writeReg(enum MFRC522_Register regAddr, uint8_t regData)
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

uint8_t RFID_readReg(enum MFRC522_Register regAddr)
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

void RFID_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize)
{
    uint8_t transferSize = writeSize + 1; // increment to include initial address byte
    uint8_t *sendBuf = malloc(transferSize);
    uint8_t *recvBuf = malloc(transferSize);
    // Address byte (See section 8.1.2.3 of Doc)
    sendBuf[0] = (REG_WRITE_OP_MASK | (FIFODataReg << REG_ADDR_BIT));
    for(int i = 0; i < writeSize; i++) {
        sendBuf[i + 1] = writeBuffer[i];
    }

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, transferSize);

    free(sendBuf);
    free(recvBuf);
}

void RFID_readFIFO(uint8_t *readBuffer, uint8_t readSize)
{
    uint8_t transferSize = readSize + 1; // increment to include initial address byte
    uint8_t *sendBuf = malloc(transferSize);
    uint8_t *recvBuf = malloc(transferSize);
    // Address byte (See section 8.1.2.3 of Doc)
    for(int i = 0; i < transferSize - 1; i++) {
        sendBuf[i] = (REG_READ_OP_MASK | (FIFODataReg << REG_ADDR_BIT));
    }
    sendBuf[transferSize - 1] = 0;

    SPI_transfer(spiFileDesc, sendBuf, recvBuf, transferSize);

    for(int i = 0; i < readSize; i++) {
        readBuffer[i] = recvBuf[i + 1];
    }

    free(sendBuf);
    free(recvBuf);
}

int RFID_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize)
{
    // Get ready for transceive
    RFID_writeReg(CollReg, Idle); // Stop Active Command
    RFID_writeReg(FIFOLevelReg, 1 << FIFOLevelReg_FLUSHBUFFER_BIT); // Flush FIFO
    RFID_writeReg(ComIrqReg, ComIrqReg_CLEAR_ALL_IRQS); // Clear all IRQs

    // Write FIFO
    RFID_writeFIFO(sendBuffer, sendSize);

    // Set Command
    RFID_writeReg(CommandReg, Transceive);

    // Start Transmission
    uint8_t regData = RFID_readReg(BitFramingReg);
    RFID_writeReg(BitFramingReg, regData | (1 << BitFramingReg_STARTSEND_BIT));

    // Poll for Receive Interrupt
    bool recvSuccess = false;
    for(int i = 0; i < 4000 && !recvSuccess; i++) {
        uint8_t irqVal = RFID_readReg(ComIrqReg);
        recvSuccess = irqVal & (ComIrqReg_RXIRQ_MASK | ComIrqReg_IDLEIRQ_MASK);
        // if(irqVal & ComIrqReg_TIMERIRQ_MASK) {
        //     return -1;
        // }
        sleepForUs(10);
    }
    if(!recvSuccess) {
        return -2;
    }
    
    // Read FIFO
    uint8_t fifoLevel = RFID_readReg(FIFOLevelReg);
    if(recvBuffer) {
        RFID_readFIFO(recvBuffer, fifoLevel);
        if(recvSize) {
            *recvSize = fifoLevel;
        }
    }
    else {
        if(recvSize) {
            *recvSize = 0;
        }
    }

    return 0;
}

void RFID_request(uint8_t req)
{
    RFID_writeReg(BitFramingReg, 0x07);

    uint8_t *sendReq = malloc(1);
    sendReq[0] = req;

    // (status, backData, backBits) = self.MFRC522_ToCard(self.PCD_TRANSCEIVE, TagType)
    int status = RFID_transceive(sendReq, 1, NULL, 0);
    printf("request status: %d\n", status);
}

void RFID_antiColl(uint8_t *uidBuffer, uint8_t *uidSize)
{
    #define BUFFER_LENGTH 9
    uint8_t *sendBuf = malloc(BUFFER_LENGTH);
    memset(sendBuf, 0, BUFFER_LENGTH);
    sendBuf[0] = PICC_SEL_CL1;
    sendBuf[1] = 0x20; // Number of valid bytes in buffer byte: 4:7 = num complete Bytes, 0:3 = extra bits

    for(int i = 0; i < BUFFER_LENGTH; i++) {
        printf("%02x ", sendBuf[i]);
    }
    printf("\n");

    uint8_t *recvBuf = malloc(BUFFER_LENGTH);

    int status = RFID_transceive(sendBuf, BUFFER_LENGTH, recvBuf, NULL);
    printf("antiColl status: %d\n", status);

    if(uidSize && *uidSize > BUFFER_LENGTH) {
        *uidSize = BUFFER_LENGTH;
    }
    for(int i = 0; i < *uidSize; i++) {
        uidBuffer[i] = recvBuf[i];
    }

    free(sendBuf);
    free(recvBuf);
}

void testFIFO(void)
{
    RFID_init(1, 0, "p9_11", 30);

    uint8_t size = 6;

    uint8_t *writeBuffer = malloc(size);
    uint8_t *readBuffer = malloc(size);

    for(int i = 0; i < size; i++) {
        writeBuffer[i] = 2 * i + 3;
    }

    RFID_writeFIFO(writeBuffer, size);
    RFID_readFIFO(readBuffer, size);
    
    for(int i = 0; i < size; i++) {
        printf("%d ", readBuffer[i]);
    }
    printf("\n");
}

void testAntiColl(void)
{
    RFID_init(1, 0, "p9_11", 30);

    uint8_t readSize = 16;

    uint8_t *readBuffer = malloc(readSize);

    RFID_request(PICC_REQA);

    RFID_antiColl(readBuffer, &readSize);

    for(int i = 0; i < readSize; i++) {
        printf("%02x ", readBuffer[i]);
    }
    printf("\n");
}

void RFID_test(void)
{
    // testFIFO();
    testAntiColl();
}