
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
 *  
 * For reference:
 * - PCD: Proximity Coupling Device (RFID Reader Chip)
 * - PICC: Proximity Inductive Coupling Card (RFID Tag)
 */

void RFID_init(int init_spiBusNum, int init_spiChipSelect, char* init_rstPin, int init_rstGpioNum);

void RFID_writeReg(enum MFRC522_Register regAddr, uint8_t regData);

uint8_t RFID_readReg(enum MFRC522_Register regAddr);

void RFID_writeFIFO(uint8_t *writeBuffer, uint8_t writeSize);

void RFID_readFIFO(uint8_t *readBuffer, uint8_t readSize);

int RFID_transceive(uint8_t *sendBuffer, uint8_t sendSize, uint8_t *recvBuffer, uint8_t *recvSize);

void RFID_test(void);


#endif