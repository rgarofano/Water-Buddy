
#include "Scale.h"

#include <stdbool.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/GPIO.h"

#define BITS_PER_INT 32

static int clkGPIO = 0;    
static int dataGPIO = 0;
static int reqGPIO = 0;

static bool initialized = false;

void Scale_init(char* init_clkPin, char* init_dataPin, char* init_reqPin, int init_clkGPIO, int init_dataGPIO, int init_reqGPIO)
{
    reqGPIO = init_reqGPIO;
    dataGPIO = init_dataGPIO;
    clkGPIO = init_clkGPIO;

    GPIO_configPin(init_reqPin, init_reqGPIO, GPIO_OUT);
    GPIO_configPin(init_clkPin, init_clkGPIO, GPIO_OUT);
    GPIO_configPin(init_dataPin, init_dataGPIO, GPIO_IN);

    initialized = true;
}

int Scale_getWeightGrams(void)
{
    if(!initialized) {
        return 0;
    }

    GPIO_write(clkGPIO, 0);
    GPIO_write(reqGPIO, 1);

    sleepForUs(REQ_WAIT_TIME_US);

    GPIO_write(reqGPIO, 0);

    int weightData = 0;

    for(int i = 0; i < BITS_PER_INT; i++) {
        GPIO_write(clkGPIO, 1);
        sleepForUs(CLK_HALF_PERIOD_US);

        GPIO_write(clkGPIO, 0);
        sleepForUs(CLK_HALF_PERIOD_US);

        int bit = GPIO_read(dataGPIO);
        weightData = (weightData << 1) + bit;
    }

    return weightData;
}
