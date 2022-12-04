
#include "Scale.h"

#include <stdbool.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/GPIO.h"

#define BITS_PER_INT 32

static int reqGPIO = 0;
static int ackGPIO = 0;
static int clkGPIO = 0;    
static int dataGPIO = 0;

static bool initialized = false;

void Scale_init(char*   init_reqPin,
                int     init_reqGPIO,
                char*   init_ackPin,
                int     init_ackGPIO,
                char*   init_clkPin,
                int     init_clkGPIO,
                char*   init_dataPin,
                int     init_dataGPIO
)
{
    reqGPIO = init_reqGPIO;
    ackGPIO = init_ackGPIO;
    clkGPIO = init_clkGPIO;
    dataGPIO = init_dataGPIO;

    GPIO_configPin(init_reqPin, reqGPIO, GPIO_OUT);
    GPIO_configPin(init_ackPin, ackGPIO, GPIO_IN);
    GPIO_configPin(init_clkPin, clkGPIO, GPIO_OUT);
    GPIO_configPin(init_dataPin, dataGPIO, GPIO_IN);

    GPIO_write(reqGPIO, 0);
    GPIO_write(clkGPIO, 0);

    initialized = true;
}

int Scale_getWeightGrams(void)
{
    if(!initialized) {
        return 0;
    }

    // Request
    GPIO_write(reqGPIO, 1);

    // Wait for acknowledge
    while(GPIO_read(ackGPIO) == 0);

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
