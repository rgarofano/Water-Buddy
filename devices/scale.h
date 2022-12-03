#ifndef SCALE_H
#define SCALE_H

//intialzes pins for BBG to arduino communication
void Scale_init(char* init_clk, char* init_data, char* init_req, int init_clkGPIO, int init_dataGPIO, int init_reqGPIO);
// returns value from scale
int Scale_getWeightGrams(void);


#endif