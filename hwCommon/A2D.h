
#ifndef A2D_H
#define A2D_H

/**
 * A2D Module
 * Provides an interface for reading the raw A2D or voltage from any analog pins on the Beaglebone.
 */

// Analog Channels are 0-indexed
#define NUM_ANALOG_CHANNELS 7
#define MAX_A2D_VALUE 4095.0f // This is a float so that float division can be done without casting
#define MAX_AIN_VOLTAGE 1.8f

// Returns the A2D value read from the specified analog channel, which is an integer in the range 0 to MAX_A2D_VALUE
// Analog channels are in the range 0 to 6
int A2D_read(int analogNum);

// Returns the voltage read from the specified analog channel, which is a floating point number in the range 0 to MAX_AIN_VOLTAGE Volts
// Analog channels are in the range 0 to 6
float A2D_readVoltage(int analogChannel);

#endif