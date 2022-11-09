
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include "SystemTools.h"
#include "GPIO.h"
#include "SPI.h"

#include "MFRC522.h"



static int      spiBusNum;
static int      spiChipSelect;
static char*    rstPin;
static int      rstGpioNum;

static int spiFileDesc = -1;



// Pin Defines
#define READER_SPI_NUM 1

#define READER_RST_PIN "p9_11"
#define READER_RST_GPIO_NUM 30

void Reader_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum)
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
    Reader_writeReg(TModeReg, 0x80);
    Reader_writeReg(TPrescalerReg, 0xA9);
    Reader_writeReg(TReloadRegH, 0x03);
    Reader_writeReg(TReloadRegL, 0xE8);
    Reader_writeReg(TxASKReg, 0x40);
    Reader_writeReg(ModeReg, 0x3D);

    // Turn antenna on
    uint8_t regVal = Reader_readReg(TxControlReg);
    Reader_writeReg(TxControlReg, regVal | 0x03);
    sleepForMs(1);
}

void Reader_writeReg(enum MFRC522_Register regAddr, uint8_t regData)
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

uint8_t Reader_readReg(enum MFRC522_Register regAddr)
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

void Reader_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize)
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

void Reader_readFIFO(uint8_t *readBuffer, uint8_t readSize)
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

enum MFRC522_StatusCode Reader_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize)
{
    // Get ready for transceive
    Reader_writeReg(CollReg, Idle); // Stop Active Command
    Reader_writeReg(FIFOLevelReg, 1 << FIFOLevelReg_FLUSHBUFFER_BIT); // Flush FIFO
    Reader_writeReg(ComIrqReg, ComIrqReg_CLEAR_ALL_IRQS); // Clear all IRQs

    // Write FIFO
    Reader_writeFIFO(sendBuffer, sendSize);

    // Set Command
    Reader_writeReg(CommandReg, Transceive);

    // Start Transmission
    uint8_t regData = Reader_readReg(BitFramingReg);
    Reader_writeReg(BitFramingReg, regData | (1 << BitFramingReg_STARTSEND_BIT));

    // Poll for Receive Interrupt
    bool recvSuccess = false;
    for(int i = 0; i < 2000 && !recvSuccess; i++) {
        uint8_t irqVal = Reader_readReg(ComIrqReg);
        recvSuccess = irqVal & (ComIrqReg_RXIRQ_MASK | ComIrqReg_IDLEIRQ_MASK);
        // if(irqVal & ComIrqReg_TIMERIRQ_MASK) {
        //     return -1;
        // }
        sleepForUs(10);
    }
    if(!recvSuccess) {
        Reader_writeReg(CommandReg, Idle);
        return STATUS_TIMEOUT;
    }
    
    // Read FIFO
    uint8_t fifoLevel = Reader_readReg(FIFOLevelReg);
    if(recvBuffer) {
        Reader_readFIFO(recvBuffer, fifoLevel);
        if(recvSize) {
            *recvSize = fifoLevel;
        }
    }
    else if(recvSize) {
        *recvSize = 0;
    }

    return STATUS_OK;
}

enum MFRC522_StatusCode Reader_piccRequest(enum MFRC522_PICC_Command piccRequest)
{
    Reader_writeReg(BitFramingReg, 0x07);
    uint8_t sendReq = (uint8_t)piccRequest;
    return Reader_transceive(&sendReq, 1, NULL, 0);
}

enum MFRC522_StatusCode Reader_getUID(uint8_t *uidBuffer, uint8_t *uidSize)
{
    #define SEND_LENGTH 2
    #define RECV_LENGTH 5
    uint8_t *sendBuf = malloc(SEND_LENGTH);
    uint8_t *recvBuf = malloc(RECV_LENGTH);
    uint8_t recvSize = RECV_LENGTH;

    sendBuf[0] = PICC_SEL_CL1;
    sendBuf[1] = 0x20; // Number of valid bytes in buffer byte: 4:7 = num complete Bytes, 0:3 = extra bits

    Reader_writeReg(BitFramingReg, 0x00);

    enum MFRC522_StatusCode status = Reader_transceive(sendBuf, SEND_LENGTH, recvBuf, &recvSize);

    if(recvSize != RECV_LENGTH) {
        return STATUS_ERROR;
    }

    if(uidSize && *uidSize > RECV_LENGTH) {
        *uidSize = RECV_LENGTH;
    }
    for(int i = 0; i < *uidSize; i++) {
        uidBuffer[i] = recvBuf[i];
    }

    free(sendBuf);
    free(recvBuf);

    return status;
}

void testFIFO(void)
{
    Reader_init(1, 0, "p9_11", 30);

    uint8_t size = 6;

    uint8_t *writeBuffer = malloc(size);
    uint8_t *readBuffer = malloc(size);

    for(int i = 0; i < size; i++) {
        writeBuffer[i] = 2 * i + 3;
    }

    Reader_writeFIFO(writeBuffer, size);
    Reader_readFIFO(readBuffer, size);
    
    for(int i = 0; i < size; i++) {
        printf("%d ", readBuffer[i]);
    }
    printf("\n");
}

void testAntiColl(void)
{
    Reader_init(1, 0, "p9_11", 30);

    uint8_t readSize = 16;

    uint8_t *readBuffer = malloc(readSize);

    Reader_piccRequest(PICC_REQA);

    Reader_getUID(readBuffer, &readSize);

    for(int i = 0; i < readSize; i++) {
        printf("%02x ", readBuffer[i]);
    }
    printf("\n");
}

void Reader_test(void)
{
    testAntiColl();
}