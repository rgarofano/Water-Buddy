
#include <stdio.h>
#include <stdbool.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/A2D.h"
#include "../hwCommon/GPIO.h"

#define ADD0_PIN "p9_13"
#define ADSK_PIN "p9_11"
#define ADD0_GPIO_NUM 31
#define ADSK_GPIO_NUM 30 

unsigned long ReadCount(void) {
    unsigned long Count; 
    unsigned char i;

    // ADDO = 1; 
    // GPIO_write(ADD0_GPIO_NUM, 1); // This is wrong

    // ADSK = 0;
    GPIO_write(ADSK_GPIO_NUM, 0);

    Count = 0;

    while(GPIO_read(ADD0_GPIO_NUM)) {
        printf("%d", GPIO_read(ADD0_GPIO_NUM));
        sleepForMs(10);
    }

    for (i = 0; i < 24; i++){ 
        Count = Count << 1;

        // ADSK=1;
        GPIO_write(ADSK_GPIO_NUM, 1);
        sleepForUs(3);
        // ADSK = 0;
        GPIO_write(ADSK_GPIO_NUM, 0);
        sleepForUs(3);

        if(GPIO_read(ADD0_GPIO_NUM)) {
            Count++;
        }
    }

    // ADSK = 1;
    GPIO_write(ADSK_GPIO_NUM, 1);

    Count = Count ^ 0x800000; 

    // ADSK = 0;
    GPIO_write(ADSK_GPIO_NUM, 0);

    return(Count); 
}

int main()
{
    GPIO_configPin(ADSK_PIN, ADSK_GPIO_NUM, GPIO_OUT);
    GPIO_configPin(ADD0_PIN, ADD0_GPIO_NUM, GPIO_IN);
    while(true) {
        // printf("%ld\n", ReadCount());
        int read0 = A2D_read(0);
        int read1 = A2D_read(1);
        int voltage = read1 - read0;
        printf("%d\n", voltage);
        sleepForMs(100);
    }
}