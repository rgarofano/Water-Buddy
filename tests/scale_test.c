
#include "../hwCommon/SystemTools.h"
#include "../hwCommon/SPI.h"
#include "../hwCommon/GPIO.h"

#define CLK_PIN     "p9_11"
#define DATA_PIN    "p9_13"
#define REQ_PIN     "p9_15"
#define CLK_GPIO    30
#define DATA_GPIO   31
#define REQ_GPIO    48

#define BITS_PER_INT 32

void init(void)
{
    GPIO_configPin(REQ_PIN, REQ_GPIO, GPIO_OUT);
    GPIO_configPin(CLK_PIN, CLK_GPIO, GPIO_OUT);
    GPIO_configPin(DATA_PIN, DATA_GPIO, GPIO_IN);
}

int getWeightGrams(void)
{
    GPIO_write(CLK_GPIO, 0);
    GPIO_write(REQ_GPIO, 1);

    sleepForMs(1);

    GPIO_write(REQ_GPIO, 0);

    int weightData = 0;

    for(int i = 0; i < BITS_PER_INT; i++) {
        GPIO_write(CLK_GPIO, 1);
        sleepForUs(1);

        GPIO_write(CLK_GPIO, 0);
        sleepForUs(1);

        int bit = GPIO_read(DATA_GPIO);
        weightData = (weightData << 1) + bit;
    }

    return weightData;
}

void loop(void)
{
    int weight = getWeightGrams();
    printf("%d\n", weight);
}

int main()
{
    init();
    while(1) {
        loop();
    }
}