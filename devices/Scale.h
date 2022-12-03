
#ifndef SCALE_H
#define SCALE_H

#define CLK_HALF_PERIOD_US 1

// Intialzes pins for BBG to arduino communication
void Scale_init(char*   init_reqPin,
                int     init_reqGPIO,
                char*   init_ackPin,
                int     init_ackGPIO,
                char*   init_clkPin,
                int     init_clkGPIO,
                char*   init_dataPin,
                int     init_dataGPIO
);

// Returns weight read by the scale in Grams
int Scale_getWeightGrams(void);


#endif