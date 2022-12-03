
#include "../devices/Scale.h"

#include <stdio.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/SPI.h"
#include "../hwCommon/GPIO.h"

#define CLK_PIN     "p9_11"
#define DATA_PIN    "p9_13"
#define REQ_PIN     "p9_15"
#define CLK_GPIO    30
#define DATA_GPIO   31
#define REQ_GPIO    48

int main()
{
    Scale_init(CLK_PIN, DATA_PIN, REQ_PIN, CLK_GPIO, DATA_GPIO, REQ_GPIO);
    
    while(1) {
        int weight = Scale_getWeightGrams();
        printf("%d\n", weight);
    }
}