
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/GPIO.h"
#include "../hwCommon/SPI.h"

#include "RFIDReader.h"



static int      spiBusNum;
static int      spiChipSelect;
static char*    rstPin;
static int      rstGpioNum;

static int spiFileDesc = -1;



void RFIDReader_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum)
{
    spiBusNum       = init_spiBusNum;
    spiChipSelect   = init_spiChipSelect;
    rstPin          = init_rstPin;
    rstGpioNum      = init_rstGpioNum;


    GPIO_configPin(rstPin, rstGpioNum, GPIO_OUT);
    GPIO_write(rstGpioNum, 0);
    sleepForUs(2);
    GPIO_write(rstGpioNum, 1);

    spiFileDesc = SPI_initPort(
        spiBusNum,
        spiChipSelect,
        SPI_MODE_DEFAULT,
        BITS_PER_WORD_DEFAULT,
        SPEED_HZ_DEFAULT
    );

    // Setup Timer Unit
    #define TIMER_DELAY_S 0.050f // 50 ms
    // Calculate required register values
    const short TPRESCALER_VAL = 0x400; // Arbitrary, but less than 0x1000 (12 bits)
    const uint8_t TPRESCALER_VAL_H = (TPRESCALER_VAL / 0x100) % 0x10; // Upper 4 bits
    const uint8_t TPRESCALER_VAL_L = TPRESCALER_VAL % 0x100; // Lower 8 bits
    // Formula for calculating required timer reload value (See section 8.5 of data sheet)
    const short TRELOAD_VAL = (short)(TIMER_DELAY_S * TIMER_CLK_FREQ_HZ / (2 * TPRESCALER_VAL + 1) - 1);
    const uint8_t TRELOAD_VAL_H = (uint8_t)(TRELOAD_VAL / 0x100); // Upper byte
    const uint8_t TRELOAD_VAL_L = ((uint8_t)TRELOAD_VAL % 0x100); // Lower byte
    // Set Timer mode to automatically start after transmission
    RFIDReader_writeReg(TModeReg, (1 << TMODEREG_TAUTO_BIT) | (TPRESCALER_VAL_H << TMODEREG_TPRESCALER_HI_BIT)); // Timer mode and upper 4 bits of TPrescalerReg
    RFIDReader_writeReg(TPrescalerReg, TPRESCALER_VAL_L);
    RFIDReader_writeReg(TReloadRegH, TRELOAD_VAL_H);
    RFIDReader_writeReg(TReloadRegL, TRELOAD_VAL_L);

    // The following settings were taken from the Arduio Library
    // Force 100% ASK modulation
    RFIDReader_writeReg(TxASKReg, 1 << TXASKREG_FORCE100ASK_BIT);
    // Set Transmission/Receiving Mode Settings
    RFIDReader_writeReg(ModeReg,
        (1 << MODEREG_TXWAITRF_BIT) | // Wait to transmit until RF field is generated
        (1 << MODEREG_POLMFIN_BIT)  | // Set polarity of MFIN to active high
        (1 << MODEREG_CRCPRESET_BIT)  // Set CRC coprocessor preset value to the one at index 1
    );

    // Turn antenna on
    uint8_t txControlRegVal = RFIDReader_readReg(TxControlReg);
    RFIDReader_writeReg(TxControlReg, txControlRegVal | TXCONTROLREG_ANTENNA_ON_MASK);
}

void RFIDReader_writeReg(enum MFRC522_Register regAddr, uint8_t regData)
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

uint8_t RFIDReader_readReg(enum MFRC522_Register regAddr)
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

void RFIDReader_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize)
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

void RFIDReader_readFIFO(uint8_t *readBuffer, uint8_t readSize)
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

enum MFRC522_StatusCode RFIDReader_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize)
{
    // Get ready for transceive
    RFIDReader_writeReg(CommandReg, Idle); // Stop Active Command
    RFIDReader_writeReg(FIFOLevelReg, 1 << FIFOLEVELREG_FLUSHBUFFER_BIT); // Flush FIFO
    RFIDReader_writeReg(ComIrqReg, COMIRQREG_CLEAR_ALL_IRQS); // Clear all IRQs

    // Write FIFO
    RFIDReader_writeFIFO(sendBuffer, sendSize);

    // Set Command
    RFIDReader_writeReg(CommandReg, Transceive);

    // Start Transmission
    uint8_t bitFramingRegVal = RFIDReader_readReg(BitFramingReg);
    RFIDReader_writeReg(BitFramingReg, bitFramingRegVal | (1 << BITFRAMINGREG_STARTSEND_BIT));

    enum MFRC522_StatusCode status = STATUS_OK;

    // Poll for Receive Interrupt
    while(true) {
        sleepForUs(10); // Arbitrary wait time
        uint8_t irqVal = RFIDReader_readReg(ComIrqReg);
        // Check receive interrupt or command state change interrupt
        if(irqVal & (COMIRQREG_RXIRQ_MASK | COMIRQREG_IDLEIRQ_MASK)) {
            break;
        }
        if(irqVal & COMIRQREG_TIMERIRQ_MASK) {
            status = STATUS_TIMEOUT;
            break;
        }
    }

    RFIDReader_writeReg(BitFramingReg, 0x00); // Stop Transmission
    RFIDReader_writeReg(CommandReg, Idle); // Stop Transceive Command
    
    // Read FIFO
    uint8_t fifoLevel = RFIDReader_readReg(FIFOLevelReg);
    if(status == STATUS_OK && recvSize) {
        if(*recvSize > fifoLevel) {
            *recvSize = fifoLevel;
        }
        if(recvBuffer) {
            RFIDReader_readFIFO(recvBuffer, *recvSize);
        }
    }

    return status;
}

enum MFRC522_StatusCode RFIDReader_sendPiccCommand(enum MFRC522_PICC_Command piccCommand)
{
    #define BITS_TO_SEND 7 // Taken from the Arduino Library
    RFIDReader_writeReg(BitFramingReg, BITS_TO_SEND << BITFRAMINGREG_TXLASTBITS_BIT);
    uint8_t sendReq = (uint8_t)piccCommand;
    return RFIDReader_transceive(&sendReq, 1, NULL, 0);
}

enum MFRC522_StatusCode RFIDReader_selectPICCAndGetUID(uint64_t *uid)
{
    if(!uid) {
        return STATUS_NO_ROOM;
    }

    #define SEND_LENGTH 2
    #define RECV_LENGTH UID_SIZE
    uint8_t *sendBuf = malloc(SEND_LENGTH);
    uint8_t *recvBuf = malloc(RECV_LENGTH);
    uint8_t recvSize = RECV_LENGTH;

    sendBuf[0] = PICC_SEL_CL1;
    sendBuf[1] = 2 << ANTICOLL_BUF_B2_VALID_BYTES_IN_BUFFER_BIT; // Number of valid bytes in buffer byte: 4:7 = num complete Bytes, 0:3 = extra bits

    enum MFRC522_StatusCode status = RFIDReader_transceive(sendBuf, SEND_LENGTH, recvBuf, &recvSize);

    if(recvSize < RECV_LENGTH) {
        free(sendBuf);
        free(recvBuf);
        return STATUS_ERROR;
    }

    *uid = 0;
    for(int i = 0; i < RECV_LENGTH; i++) {
        *uid = *uid << BITS_PER_BYTE;
        *uid += recvBuf[i];
    }

    free(sendBuf);
    free(recvBuf);

    return status;
}

enum MFRC522_StatusCode RFIDReader_getImmediateUID(uint64_t *uid)
{
    enum MFRC522_StatusCode status = RFIDReader_sendPiccCommand(PICC_REQA);
    if(status != STATUS_OK) {
        return status;
    }

    status = RFIDReader_selectPICCAndGetUID(uid);
    return status;
}

bool RFIDReader_isTagPresent(void)
{
    uint64_t tempUid = 0;
    enum MFRC522_StatusCode status = RFIDReader_getImmediateUID(&tempUid);
    return ((status == STATUS_OK) && (tempUid != 0));
}
