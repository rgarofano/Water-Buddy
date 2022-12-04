
#ifndef SYSTEMTOOLS_H
#define SYSTEMTOOLS_H

/**
 * System Tools Module
 * Contains various useful functions.
 * Includes functions for:
 * - Getting current time
 * - Sleeping for a specified time
 * - Opening a file with a check
 * - Byte rotating
 * - Configuring pins on the Beaglebone
 */

#include <stdio.h>

#define MS_PER_S    1000

#define US_PER_S    1000000
#define US_PER_MS   1000

#define NS_PER_S    1000000000
#define NS_PER_MS   1000000
#define NS_PER_US   1000

#define MS_PER_HOUR 3600000

// Returns current time in milliseconds
// Copied from Assignment 1 Document
long long getTimeInMs(void);

// Returns current time in microseconds
long long getTimeInUs(void);

double getTimeInHours(void);

// Copied from Assignment 1 Document
void sleepForMs(long long sleepTimeMs);

void sleepForUs(long long sleepTimeUs);

// Attempts to open file fileName and checks if it succeeded.
// Exits with exitcode 1 if it fails.
FILE* fopenWithCheck(const char* fileName, const char* accessType);

// Bit Operations
#define BITS_PER_BYTE 8

// Returns the byte rotated to the right by n bits
char rotateCharRight(char byte, int n);

// Returns the byte rotated to the left by n bits
char rotateCharLeft(char bbyte, int n);

// Pin Config
void configPin(const char* pin, const char* config);

#endif