
#ifndef MFRC522_H
#define MFRC522_H

#include <stdint.h>

#include "RFIDReader_data.h"

/**
 * This module provides an API for the MFRC522 RFID Reader
 * The information in this file is informed by the data sheet and the Arduino Library
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
 * For reference:
 * - PCD: Proximity Coupling Device (RFID Reader Chip)
 * - PICC: Proximity Inductive Coupling Card (RFID Tag)
 */

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

void RFIDReader_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum);

void RFIDReader_writeReg(enum MFRC522_Register regAddr, uint8_t regData);

uint8_t RFIDReader_readReg(enum MFRC522_Register regAddr);

void RFIDReader_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize);

void RFIDReader_readFIFO(uint8_t *readBuffer, uint8_t readSize);

enum MFRC522_StatusCode RFIDReader_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize);

enum MFRC522_StatusCode RFIDReader_piccRequest(enum MFRC522_PICC_Command piccRequest);

enum MFRC522_StatusCode RFIDReader_selectPICCAndGetUID(uint64_t *uid);

enum MFRC522_StatusCode RFIDReader_getImmediateUID(uint64_t *uid);

#endif