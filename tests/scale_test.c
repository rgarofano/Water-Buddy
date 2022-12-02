



#include "../hwCommon/SystemTools.h"
#include "../hwCommon/SPI.h"
#include "../hwCommon/GPIO.h"

#define CLK_PIN     "p9_11"
#define DATA_PIN    "p9_13"
#define REQ_PIN     "p9_15"
#define CLK_GPIO    30
#define DATA_GPIO   31
#define REQ_GPIO    48

// static int fileDesc = 0;

void init(void)
{
    // fileDesc = SPI_initPort(1, 1, SPI_MODE_DEFAULT, BITS_PER_WORD_DEFAULT, SPEED_HZ_DEFAULT);
    GPIO_configPin(REQ_PIN, REQ_GPIO, GPIO_OUT);
    GPIO_configPin(CLK_PIN, CLK_GPIO, GPIO_OUT);
    GPIO_configPin(DATA_PIN, DATA_GPIO, GPIO_IN);
}

float getWeight(void)
{
    GPIO_write(CLK_GPIO, 0);
    GPIO_write(REQ_GPIO, 1);

    sleepForMs(100);

    GPIO_write(REQ_GPIO, 0);

    int weightData = 0;

    for(int i = 0; i < 32; i++) {
        GPIO_write(CLK_GPIO, 1);
        sleepForUs(100);

        GPIO_write(CLK_GPIO, 0);
        sleepForUs(100);

        int bit = GPIO_read(DATA_GPIO);
        printf("%d", bit);
        fflush(stdout);

        weightData = (weightData << 1) + bit;
    }

    printf("\n");

    float weight = (float)weightData;

    return weight;
}

void loop(void)
{
    sleepForMs(500);

    float weight = getWeight();

    printf("%f\n", weight);
}

// 520 - 340 = 180 ms for bbg to print after arduino writing

int main()
{
    init();
    while(1) {
        loop();
    }
}