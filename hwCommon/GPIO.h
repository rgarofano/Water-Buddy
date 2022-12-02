
#ifndef GPIO_H
#define GPIO_H

/**
 * GPIO Module
 * Provides an interface for configuring, reading, and writing any of the GPIO pins or LEDS on the Beaglebone.
 */

#include <stdint.h>

#define GPIO_IN "in"
#define GPIO_OUT "out"

// A more specific version of configPin(pin, config)
void GPIO_configPin(const char* pin, const int gpioNum, const char *direction);

int GPIO_read(int gpioNum);

void GPIO_write(int gpioNum, int value);



// LED
#define LED_ON "1"
#define LED_OFF "0"

// static const char *LED_TRIGGER_DEFAULT[] = {"heartbeat", "mmc0", "cpu0", "mmc1"};

// LED number is in the range 0 to 3
void LED_setAttribute(const int LEDNum, const char* attribute, const char* value);

#endif