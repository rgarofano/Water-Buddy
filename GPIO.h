
#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
 * This header file includes general functions for:
 * GPIO
 * Beaglebone's LEDs
 */



// GPIO
#define GPIO_IN "in"
#define GPIO_OUT "out"

// A more specific version of configPin(pin, config)
void configPinForGPIO(const char* pin, const int gpioNum, const char *direction);

int gpioRead(int gpioNum);

void gpioWrite(int gpioNum, int value);



// LED
#define LED_ON "1"
#define LED_OFF "0"

// static const char *LED_TRIGGER_DEFAULT[] = {"heartbeat", "mmc0", "cpu0", "mmc1"};

// LED number is in the range 0 to 3
void setLEDAttribute(const int LEDNum, const char* attribute, const char* value);

#endif