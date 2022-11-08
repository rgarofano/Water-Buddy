
#ifndef MFRC522_H
#define MFRC522_H

/**
 * This module provides an API for the MFRC522 RFID Reader
 * The information in this file is informed by the data sheet and the Arduino Library
 * 
 * Here is the Arduino library for the MFRC522:
 * https://github.com/miguelbalboa/rfid/tree/master/src
 * Items taken from the Arduino library:
 * - Command enum table
 * - Register enum table
 */

// Command Set (Section 10, page 70 of data sheet)
enum MFRC522_Command {
    Idle				= 0x00,		// no action, cancels current command execution
	Mem					= 0x01,		// stores 25 bytes into the internal buffer
	GenerateRandomID	= 0x02,		// generates a 10-byte random ID number
	CalcCRC				= 0x03,		// activates the CRC coprocessor or performs a self-test
	Transmit			= 0x04,		// transmits data from the FIFO buffer
	NoCmdChange			= 0x07,		// no command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit
	Receive				= 0x08,		// activates the receiver circuits
	Transceive 			= 0x0C,		// transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
	MFAuthent 			= 0x0E,		// performs the MIFARE standard authentication as a reader
	SoftReset			= 0x0F	    // resets the MFRC522
};

enum MFRC522_Register {
    // Page 0: Command and Status
    // Reserved				  0x00,	// reserved for future use
    CommandReg				= 0x01,	// starts and stops command execution
    ComIEnReg				= 0x02,	// enable and disable interrupt request control bits
    DivIEnReg				= 0x03,	// enable and disable interrupt request control bits
    ComIrqReg				= 0x04,	// interrupt request bits
    DivIrqReg				= 0x05,	// interrupt request bits
    ErrorReg				= 0x06,	// error bits showing the error status of the last command executed 
    Status1Reg				= 0x07,	// communication status bits
    Status2Reg				= 0x08,	// receiver and transmitter status bits
    FIFODataReg				= 0x09,	// input and output of 64 byte FIFO buffer
    FIFOLevelReg			= 0x0A,	// number of bytes stored in the FIFO buffer
    WaterLevelReg			= 0x0B,	// level for FIFO underflow and overflow warning
    ControlReg				= 0x0C,	// miscellaneous control registers
    BitFramingReg			= 0x0D,	// adjustments for bit-oriented frames
    CollReg					= 0x0E,	// bit position of the first bit-collision detected on the RF interface
    // Reserved			      0x0F	// reserved for future use
    
    // Page 1: Command
    // Reserved 			  0x10,	// reserved for future use
    ModeReg					= 0x11,	// defines general modes for transmitting and receiving 
    TxModeReg				= 0x12,	// defines transmission data rate and framing
    RxModeReg				= 0x13,	// defines reception data rate and framing
    TxControlReg			= 0x14,	// controls the logical behavior of the antenna driver pins TX1 and TX2
    TxASKReg				= 0x15,	// controls the setting of the transmission modulation
    TxSelReg				= 0x16,	// selects the internal sources for the antenna driver
    RxSelReg				= 0x17,	// selects internal receiver settings
    RxThresholdReg			= 0x18,	// selects thresholds for the bit decoder
    DemodReg				= 0x19,	// defines demodulator settings
    // Reserved 		      0x1A	// reserved for future use
    // Reserved				  0x1B	// reserved for future use
    MfTxReg					= 0x1C,	// controls some MIFARE communication transmit parameters
    MfRxReg					= 0x1D,	// controls some MIFARE communication receive parameters
    // Reserved				  0x1E	// reserved for future use
    SerialSpeedReg			= 0x1F,	// selects the speed of the serial UART interface
    
    // Page 2: Configuration
    // Reserved				  0x20	// reserved for future use
    CRCResultRegH			= 0x21,	// shows the MSB and LSB values of the CRC calculation
    CRCResultRegL			= 0x22,
    // Reserved				  0x23	// reserved for future use
    ModWidthReg				= 0x24,	// controls the ModWidth setting?
    // Reserved				  0x25	// reserved for future use
    RFCfgReg				= 0x26,	// configures the receiver gain
    GsNReg					= 0x27,	// selects the conductance of the antenna driver pins TX1 and TX2 for modulation 
    CWGsPReg				= 0x28,	// defines the conductance of the p-driver output during periods of no modulation
    ModGsPReg				= 0x29,	// defines the conductance of the p-driver output during periods of modulation
    TModeReg				= 0x2A,	// defines settings for the internal timer
    TPrescalerReg			= 0x2B,	// the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
    TReloadRegH				= 0x2C,	// defines the 16-bit timer reload value
    TReloadRegL				= 0x2D,
    TCounterValueRegH		= 0x2E,	// shows the 16-bit timer value
    TCounterValueRegL		= 0x2F,
    
    // Page 3: Test Registers
    // Reserved				  0x30	// reserved for future use
    TestSel1Reg				= 0x31,	// general test signal configuration
    TestSel2Reg				= 0x32,	// general test signal configuration
    TestPinEnReg			= 0x33,	// enables pin output driver on pins D1 to D7
    TestPinValueReg			= 0x34,	// defines the values for D1 to D7 when it is used as an I/O bus
    TestBusReg				= 0x35,	// shows the status of the internal test bus
    AutoTestReg				= 0x36,	// controls the digital self-test
    VersionReg				= 0x37,	// shows the software version
    AnalogTestReg			= 0x38,	// controls the pins AUX1 and AUX2
    TestDAC1Reg				= 0x39,	// defines the test value for TestDAC1
    TestDAC2Reg				= 0x3A,	// defines the test value for TestDAC2
    TestADCReg				= 0x3B	// shows the value of ADC I and Q channels
    // Reserved				  0x3C	// reserved for production tests
    // Reserved				  0x3D	// reserved for production tests
    // Reserved				  0x3E	// reserved for production tests
    // Reserved				  0x3F	// reserved for production tests
};

void RFID_init(int init_spiBusNum, char* init_rstPin, int init_rstGpioNum, char* init_sdaPin
);

unsigned char RFID_readReg(unsigned char regAddr);

unsigned char RFID_writeReg(unsigned char regAddr, unsigned char regData);

void RFID_test(void);


#endif