/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017 
   -------------------------------------------------------------------------------------
*/

#ifndef HX711_ADC_H
#define HX711_ADC_H

#include "HX711_config.h"

#include <stdint.h>
#include <stdbool.h>

/*
Note: HX711_ADC configuration values has been moved to file config.h
*/

#define DATA_SET 	SAMPLES + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE // total samples in memory

#if (SAMPLES  != 1) & (SAMPLES  != 2) & (SAMPLES  != 4) & (SAMPLES  != 8) & (SAMPLES  != 16) & (SAMPLES  != 32) & (SAMPLES  != 64) & (SAMPLES  != 128)
	#error "number of SAMPLES not valid!"
#endif

#if (SAMPLES  == 1) & ((IGN_HIGH_SAMPLE  != 0) | (IGN_LOW_SAMPLE  != 0))
	#error "number of SAMPLES not valid!"
#endif

#if 		(SAMPLES == 1)
#define 	DIVB 0
#elif 		(SAMPLES == 2)
#define 	DIVB 1
#elif 		(SAMPLES == 4)
#define 	DIVB 2
#elif  		(SAMPLES == 8)
#define 	DIVB 3
#elif  		(SAMPLES == 16)
#define 	DIVB 4
#elif  		(SAMPLES == 32)
#define 	DIVB 5
#elif  		(SAMPLES == 64)
#define 	DIVB 6
#elif  		(SAMPLES == 128)
#define 	DIVB 7
#endif

#define SIGNAL_TIMEOUT	100

void 	HX711_setGain(uint8_t gain); 			//value must be 32, 64 or 128*
void	HX711_begin(char *init_doutPin, char *init_sckPin, uint8_t init_doutGpioNum, uint8_t init_sckGpioNum);
void	HX711_start(unsigned long t, bool dotare);		//start HX711, do tare if selected
void 	HX711_tare(); 								//zero the scale, wait for tare to finnish (blocking)
void 	HX711_tareNoDelay(); 						//zero the scale, initiate the tare operation to run in the background (non-blocking)
bool 	HX711_getTareStatus();						//returns 'true' if tareNoDelay() operation is complete
void 	HX711_setCalFactor(float cal); 				//set new calibration factor, raw data is divided by this value to convert to readable data
float 	HX711_getCalFactor(); 						//returns the current calibration factor
float 	HX711_getData(); 							//returns data from the moving average dataset 

int 	HX711_getReadIndex(); 						//for testing and debugging
float 	HX711_getConversionTime(); 					//for testing and debugging
float 	HX711_getSPS();								//for testing and debugging
bool 	HX711_getTareTimeoutFlag();					//for testing and debugging
void 	HX711_disableTareTimeout();					//for testing and debugging
long 	HX711_getSettlingTime();						//for testing and debugging
void 	HX711_powerDown(); 							//power down the HX711
void 	HX711_powerUp(); 							//power up the HX711
long 	HX711_getTareOffset();						//get the tare offset (raw data value output without the scale "calFactor")
void 	HX711_setTareOffset(long newoffset);			//set new tare offset (raw data value input without the scale "calFactor")
uint8_t HX711_update(); 							//if conversion is ready; read out 24 bit data and add to dataset
bool 	HX711_dataWaitingAsync(); 					//checks if data is available to read (no conversion yet)
bool 	HX711_updateAsync(); 						//read available data and add to dataset 
void 	HX711_setSamplesInUse(int samples);			//overide number of samples in use
int 	HX711_getSamplesInUse();						//returns current number of samples in use
void 	HX711_resetSamplesIndex();					//resets index for dataset
bool 	HX711_refreshDataSet();						//Fill the whole dataset up with new conversions, i.e. after a reset/restart (this function is blocking once started)
bool 	HX711_getDataSetStatus();					//returns 'true' when the whole dataset has been filled up with conversions, i.e. after a reset/restart
float 	HX711_getNewCalibration(float known_mass);	//returns and sets a new calibration value (calFactor) based on a known mass input
bool 	HX711_getSignalTimeoutFlag();				//returns 'true' if it takes longer time then 'SIGNAL_TIMEOUT' for the dout pin to go low after a new conversion is started
void 	HX711_setReverseOutput();					//reverse the output value
void 	HX711_conversion24bit(); 					//if conversion is ready: returns 24 bit data and starts the next conversion
long 	HX711_smoothedData();						//returns the smoothed data value calculated from the dataset


#endif
   
