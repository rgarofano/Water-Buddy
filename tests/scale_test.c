
#include "../devices/Scale.h"

#include <stdio.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/SPI.h"
#include "../hwCommon/GPIO.h"

#include "../WaterBuddy_pinDefines.h"

int main()
{
    Scale_init( SCALE_REQ_PIN,
                SCALE_REQ_GPIO,
                SCALE_ACK_PIN,
                SCALE_ACK_GPIO,
                SCALE_CLK_PIN,
                SCALE_CLK_GPIO,
                SCALE_DATA_PIN,
                SCALE_DATA_GPIO);
    
    while(1) {
        int weight = Scale_getWeightGrams();
        printf("%d\n", weight);
    }
}