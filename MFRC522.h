
#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

#include "MFRC522_data.h"

/**
 * This module provides an API for the MFRC522 RFID Reader
 * The information in this file is informed by the data sheet and the Arduino Library
 * 
 * Here is the Arduino library for the MFRC522:
 * https://github.com/miguelbalboa/rfid/tree/master/src
 * Items taken from the Arduino library:
 * - Register enum table (See MFRC522_data.h)
 * - PCD Command enum table (See MFRC522_data.h)
 * - PICC Command enum table (See MFRC522_data.h)
 * - Status Code enum table
 *  
 * For reference:
 * - PCD: Proximity Coupling Device (RFID Reader Chip)
 * - PICC: Proximity Inductive Coupling Card (RFID Tag)
 */

// Return codes from the functions in this class. Remember to update GetStatusCodeName() if you add more.
// last value set to 0xff, then compiler uses less ram, it seems some optimisations are triggered
enum StatusCode {
    STATUS_OK				= 0,	// Success
    STATUS_ERROR			= -1,	// Error in communication
    STATUS_COLLISION		= -2,	// Collission detected
    STATUS_TIMEOUT			= -3,	// Timeout in communication.
    STATUS_NO_ROOM			= -4,	// A buffer is not big enough.
    STATUS_INTERNAL_ERROR	= -5,	// Internal error in the code. Should not happen ;-)
    STATUS_INVALID			= -6,	// Invalid argument.
    STATUS_CRC_WRONG		= -7,	// The CRC_A does not match
    STATUS_MIFARE_NACK		= 0xff	// A MIFARE PICC responded with NAK.
};

void RFID_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum);

void RFID_writeReg(enum MFRC522_Register regAddr, uint8_t regData);

uint8_t RFID_readReg(enum MFRC522_Register regAddr);

void RFID_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize);

void RFID_readFIFO(uint8_t *readBuffer, uint8_t readSize);

enum StatusCode RFID_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize);

void RFID_test(void);


#endif