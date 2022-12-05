
#ifndef RFIDREADER_H
#define RFIDREADER_H

/**
 * RFID Reader Module
 * Provides an API for the MFRC522 RFID Reader.
 * The information in this module is informed by the data sheet and the Arduino Library.
 * 
 * Here is the Arduino library for the MFRC522:
 * https://github.com/miguelbalboa/rfid/tree/master/src
 * Items taken from the Arduino library:
 * - Register enum table (in MFRC522_data.h)
 * - PCD Command enum table (in MFRC522_data.h)
 * - PICC Command enum table (in MFRC522_data.h)
 * - Status Code enum table
 * - Transmission/Receiving Mode Settings (Set in MFRC522.c)
 *  
 * Definition of terms:
 * - PCD: Proximity Coupling Device (RFID Reader Chip)
 * - PICC: Proximity Inductive Coupling Card (RFID Tag)
 */

#include <stdint.h>
#include <stdbool.h>

#include "RFIDReader_data.h"

enum MFRC522_StatusCode {
    STATUS_OK				=  0,	// Success
    STATUS_ERROR			= -1,	// Error in communication
    STATUS_COLLISION		= -2,	// Collission detected
    STATUS_TIMEOUT			= -3,	// Timeout in communication.
    STATUS_NO_ROOM			= -4,	// A buffer is not big enough.
    STATUS_INTERNAL_ERROR	= -5,	// Internal error in the code. Should not happen ;-)
    STATUS_INVALID			= -6,	// Invalid argument.
    STATUS_CRC_WRONG		= -7,	// The CRC_A does not match
    STATUS_MIFARE_NACK		= -8	// A MIFARE PICC responded with NAK.
};

#define UID_SIZE 5

// Configures the pins associated with the specified SPI bus number for SPI
// Sets the MFRC522 reset pin high, initialized necessary registers, and turns the antenna on
void RFIDReader_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum);

void RFIDReader_writeReg(enum MFRC522_Register regAddr, uint8_t regData);

uint8_t RFIDReader_readReg(enum MFRC522_Register regAddr);

// Writes writeSize bytes of writeBuffer to MFRC522's internal FIFO in order
void RFIDReader_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize);

// Reads readSize bytes of the MFRC522's internal FIFO and writes them to readBuffer in order
// Bytes in the FIFO are cleared as they are read
void RFIDReader_readFIFO(uint8_t *readBuffer, uint8_t readSize);

// Writes sendSize bytes of sendBuffer to the MFRC522's internal FIFO in order
// Transmits all bytes in the FIFO to PICCs in the vicinity
// Receives response and stores it in recvBuffer
// Returns STATUS_OK on success and STATUS_TIMEOUT if no PICC in the vicinity
enum MFRC522_StatusCode RFIDReader_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize);

// Transmits a PICC command to PICCs in the vicinity
// Returns STATUS_OK on success and STATUS_TIMEOUT if no PICC in the vicinity
enum MFRC522_StatusCode RFIDReader_sendPiccCommand(enum MFRC522_PICC_Command piccCommand);

// Transmits a Request command to PICCs in the vicinity
// Returns STATUS_OK on success and STATUS_TIMEOUT if no PICC in the vicinity
enum MFRC522_StatusCode RFIDReader_requestPicc(void);

// Gets the UID of an active PICC in the vicinity and stores it in uid
// A Wakeup or Request PICC Command MUST succeed first. Otherwise, the returned UID will be garbage
// Returns STATUS_OK on success, STATUS_TIMEOUT if no PICC in the vicinity,
// and occasionally STATUS_ERROR as a PICC is entering or exiting the vicinity
enum MFRC522_StatusCode RFIDReader_getActivePiccUID(uint64_t *uid);

// Sends Request PICC command first, then gets the UID of the PICC in the vicinity and stores it in uid
// Returns STATUS_OK on success, STATUS_TIMEOUT if no PICC in the vicinity,
// and occasionally STATUS_ERROR as a PICC is entering or exiting the vicinity
enum MFRC522_StatusCode RFIDReader_requestPiccAndGetUID(uint64_t *uid);

bool RFIDReader_isTagPresent(void);

#endif