
#ifndef SCALE_H
#define SCALE_H

#define REQ_WAIT_TIME_US 1000

#define CLK_HALF_PERIOD_US 1

// Intialzes pins for BBG to arduino communication
void Scale_init(char* init_clkPin,
                char* init_dataPin,
                char* init_reqPin,
                int init_clkGPIO,
                int init_dataGPIO,
                int init_reqGPIO
);

// Returns weight read by the scale in Grams
int Scale_getWeightGrams(void);


#endif