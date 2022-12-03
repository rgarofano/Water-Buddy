
#include "../hwCommon/SystemTools.h"
#include "../hwCommon/SPI.h"
#include "../hwCommon/GPIO.h"

static int clkGPIO = 0;    
static int dataGPIO = 0;
static int reqGPIO = 0;


#define BITS_PER_INT 32

void Scale_init(char* init_clk, char* init_data, char* init_req, int init_clkGPIO, int init_dataGPIO, int init_reqGPIO)
{
    reqGPIO = init_reqGPIO;
    dataGPIO = init_dataGPIO;
    clkGPIO = init_clkGPIO;

    GPIO_configPin(init_req, init_reqGPIO, GPIO_OUT);
    GPIO_configPin(init_clk, init_clkGPIO, GPIO_OUT);
    GPIO_configPin(init_data, init_dataGPIO, GPIO_IN);
}

int Scale_getWeightGrams(void)
{
    GPIO_write(clkGPIO, 0);
    GPIO_write(reqGPIO, 1);

    sleepForMs(1);

    GPIO_write(reqGPIO, 0);

    int weightData = 0;

    for(int i = 0; i < BITS_PER_INT; i++) {
        GPIO_write(clkGPIO, 1);
        sleepForUs(1);

        GPIO_write(clkGPIO, 0);
        sleepForUs(1);

        int bit = GPIO_read(dataGPIO);
        weightData = (weightData << 1) + bit;
    }

    return weightData;
}

