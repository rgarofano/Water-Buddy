
#ifndef RFIDREADER_DATA_H
#define RFIDREADER_DATA_H

/**
 * RFID Reader Data
 * This header file provides information about MFRC522 registers and commands
 * The information in this file is informed by the data sheet and the Arduino Library
 * 
 * Here is the Arduino library for the MFRC522:
 * https://github.com/miguelbalboa/rfid/tree/master/src
 * Items taken from the Arduino library:
 * - Register enum table
 * - PCD Command enum table
 * - PICC Command enum table
 * - Status Code enum table (in MFRC522.h)
 * - Transmission/Receiving Mode Settings (Set in MFRC522.c)
 *  
 * Definition of terms:
 * - PCD: Proximity Coupling Device (RFID Reader Chip)
 * - PICC: Proximity Inductive Coupling Card (RFID Tag)
 */

// Register Addresses (See section 9 of data sheet)
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

// General Register Parameters
#define REG_READ_OP         1
#define REG_WRITE_OP        0
#define REG_OP_BIT          7
#define REG_READ_OP_MASK    (REG_READ_OP  << REG_OP_BIT)
#define REG_WRITE_OP_MASK   (REG_WRITE_OP << REG_OP_BIT)

#define REG_ADDR_BIT        1

// Register Specific Parameters
#define BITFRAMINGREG_STARTSEND_BIT 7
#define BITFRAMINGREG_TXLASTBITS_BIT 0

#define COMIRQREG_CLEAR_ALL_IRQS 0x7F
#define COMIRQREG_RXIRQ_MASK (1 << 5) // Bit 5
#define COMIRQREG_IDLEIRQ_MASK (1 << 4) // Bit 4
#define COMIRQREG_TIMERIRQ_MASK 1 // Bit 0

#define FIFOLEVELREG_FLUSHBUFFER_BIT 7

#define MODEREG_MSBFIRST_BIT 7
#define MODEREG_TXWAITRF_BIT 5
#define MODEREG_POLMFIN_BIT 3
#define MODEREG_CRCPRESET_BIT 0

#define TIMER_CLK_FREQ_HZ 13560000 // 13.56 MHz
#define TMODEREG_TAUTO_BIT 7
#define TMODEREG_TPRESCALER_HI_BIT 0

#define TXASKREG_FORCE100ASK_BIT 6

#define TXCONTROLREG_ANTENNA_ON_MASK 0x03



// Command Codes (See section 10 of data sheet)
enum MFRC522_PCD_Command {
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

// AntiCollision Transmission Buffer params
#define ANTICOLL_BUF_B2_VALID_BYTES_IN_BUFFER_BIT 4

// Commands sent to the PICC
// The commands used by the PCD to manage communication with several PICCs (ISO 14443-3, Type A, section 6.4)
enum MFRC522_PICC_Command {
    PICC_REQA			= 0x26, // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
    PICC_WUPA			= 0x52, // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
    PICC_CT				= 0x88, // Cascade Tag. Not really a command, but used during anti collision.
    PICC_SEL_CL1		= 0x93, // Anti collision/Select, Cascade Level 1
    PICC_SEL_CL2		= 0x95, // Anti collision/Select, Cascade Level 2
    PICC_SEL_CL3		= 0x97, // Anti collision/Select, Cascade Level 3
    PICC_HLTA			= 0x50, // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
    PICC_RATS           = 0xE0, // Request command for Answer To Reset.
    // The commands used for MIFARE Classic (from http://www.mouser.com/ds/2/302/MF1S503x-89574.pdf, Section 9)
    // Use PCD_MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
    // The read/write commands can also be used for MIFARE Ultralight.
    PICC_MF_AUTH_KEY_A	= 0x60, // Perform authentication with Key A
    PICC_MF_AUTH_KEY_B	= 0x61, // Perform authentication with Key B
    PICC_MF_READ		= 0x30, // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
    PICC_MF_WRITE		= 0xA0, // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
    PICC_MF_DECREMENT	= 0xC0, // Decrements the contents of a block and stores the result in the internal data register.
    PICC_MF_INCREMENT	= 0xC1, // Increments the contents of a block and stores the result in the internal data register.
    PICC_MF_RESTORE		= 0xC2, // Reads the contents of a block into the internal data register.
    PICC_MF_TRANSFER	= 0xB0, // Writes the contents of the internal data register to a block.
    // The commands used for MIFARE Ultralight (from http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
    // The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
    PICC_UL_WRITE       = 0xA2 // Writes one 4 byte page to the PICC.
};

#endif