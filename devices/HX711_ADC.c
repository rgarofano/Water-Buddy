
/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

#include "HX711_ADC.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/GPIO.h"

#define LOW 0
#define HIGH 1


static uint8_t sckGpioNum; 							//HX711 pd_sck pin
static uint8_t doutGpioNum; 							//HX711 dout pin
static uint8_t gainSetting;								//HX711 gainSetting
static float calFactor = 1.0;						//calibration factor as given in function setCalFactor(float cal)
static float calFactorRecip = 1.0;					//reciprocal calibration factor (1/calFactor), the HX711 raw data is multiplied by this value
static volatile long dataSampleSet[DATA_SET + 1];	// dataset, make voltile if interrupt is used 
static long tareOffset = 0;
static int readIndex = 0;
static unsigned long conversionStartTime = 0;
static unsigned long conversionTime = 0;
static uint8_t tareTimes = 0;
static uint8_t divBit = DIVB;
static const uint8_t divBitCompiled = DIVB;
static bool doTare = 0;
static uint8_t convRslt = 0;
static bool tareStatus = 0;
static const unsigned int TARE_TIMEOUT_TIME_MS = (SAMPLES + IGN_HIGH_SAMPLE + IGN_HIGH_SAMPLE) * 150; // tare timeout time in ms, no of samples * 150ms (10SPS + 50% margin)
static bool tareTimeoutFlag = 0;
static bool tareTimeoutDisable = 0;
static int samplesInUse = SAMPLES;
static long lastSmoothedData = 0;
static bool dataOutOfRange = 0;
static unsigned long lastDoutLowTime = 0;
static bool signalTimeoutFlag = 0;
static bool reverseVal = 0;
static bool dataWaiting = 0;

void HX711_setGain(uint8_t gain)  //value should be 32, 64 or 128*
{
	switch(gain) {
		case 32:
			gainSetting = 2;
			break;
		case 64:
			gainSetting = 3;
			break;
		case 128:
			gainSetting = 1;
			break;
		default:
			gainSetting = 1;
	}
}

//set pinMode, HX711 gain and power up the HX711
void HX711_init(char *init_doutPin, char *init_sckPin, uint8_t init_doutGpioNum, uint8_t init_sckGpioNum)
{
	doutGpioNum = init_doutGpioNum;
	sckGpioNum = init_sckGpioNum;
	// pinMode(sckGpioNum, OUTPUT);
	// pinMode(doutGpioNum, INPUT);
	GPIO_configPin(init_sckPin, init_sckGpioNum, GPIO_OUT);
	GPIO_configPin(init_doutPin, init_doutGpioNum, GPIO_IN);
	HX711_setGain(128);
	HX711_powerUp();
}

/*  start(t, dotare) with selectable tare:
*	will do conversions continuously for 't' +400 milliseconds (400ms is min. settling time at 10SPS). 
*   Running this for 1-5s in setup() - before tare() seems to improve the tare accuracy. */
void HX711_start(unsigned long t, bool dotare)
{
	t += 400;
	lastDoutLowTime = getTimeInMs();
	while(getTimeInMs() < t) 
	{
		HX711_update();
		sleepForUs(1);
	}
	if (dotare)
	{
		HX711_tare();
		tareStatus = 0;
	}
}	

//zero the scale, wait for tare to finnish (blocking)
void HX711_tare() 
{
	uint8_t rdy = 0;
	doTare = 1;
	tareTimes = 0;
	tareTimeoutFlag = 0;
	unsigned long timeoutMs = getTimeInMs() + TARE_TIMEOUT_TIME_MS;
	while(rdy != 2) 
	{
		rdy = HX711_update();
		if (!tareTimeoutDisable) 
		{
			if (getTimeInMs() > timeoutMs) 
			{ 
				tareTimeoutFlag = 1;
				break; // Prevent endless loop if no HX711 is connected
			}
		}
		sleepForUs(1);
	}
}

//zero the scale, initiate the tare operation to run in the background (non-blocking)
void HX711_tareNoDelay() 
{
	doTare = 1;
	tareTimes = 0;
	tareStatus = 0;
}

//set new calibration factor, raw data is divided by this value to convert to readable data
void HX711_setCalFactor(float cal) 
{
	calFactor = cal;
	calFactorRecip = 1/calFactor;
}

//returns 'true' if tareNoDelay() operation is complete
bool HX711_getTareStatus() 
{
	bool t = tareStatus;
	tareStatus = 0;
	return t;
}

//returns the current calibration factor
float HX711_getCalFactor() 
{
	return calFactor;
}

//call the function update() in loop or from ISR
//if conversion is ready; read out 24 bit data and add to dataset, returns 1
//if tare operation is complete, returns 2
//else returns 0
uint8_t HX711_update() 
{
	uint8_t dout = GPIO_read(doutGpioNum); //check if conversion is ready
	if (!dout) 
	{
		HX711_conversion24bit();
		lastDoutLowTime = getTimeInMs();
		signalTimeoutFlag = 0;
	}
	else 
	{
		//if (getTimeInMs() > (lastDoutLowTime + SIGNAL_TIMEOUT))
		if (getTimeInMs() - lastDoutLowTime > SIGNAL_TIMEOUT)
		{
			signalTimeoutFlag = 1;
		}
		convRslt = 0;
	}
	return convRslt;
}

// call the function dataWaitingAsync() in loop or from ISR to check if new data is available to read
// if conversion is ready, just call updateAsync() to read out 24 bit data and add to dataset
// returns 1 if data available , else 0
bool HX711_dataWaitingAsync() 
{
	if (dataWaiting) { lastDoutLowTime = getTimeInMs(); return 1; }
	uint8_t dout = GPIO_read(doutGpioNum); //check if conversion is ready
	if (!dout) 
	{
		dataWaiting = true;
		lastDoutLowTime = getTimeInMs();
		signalTimeoutFlag = 0;
		return 1;
	}
	else
	{
		//if (getTimeInMs() > (lastDoutLowTime + SIGNAL_TIMEOUT))
		if (getTimeInMs() - lastDoutLowTime > SIGNAL_TIMEOUT)
		{
			signalTimeoutFlag = 1;
		}
		convRslt = 0;
	}
	return 0;
}

// if data is available call updateAsync() to convert it and add it to the dataset.
// call getData() to get latest value
bool HX711_updateAsync() 
{
	if (dataWaiting) { 
		HX711_conversion24bit();
		dataWaiting = false;
		return true;
	}
	return false;

}

float HX711_getData() // return fresh data from the moving average dataset
{
	long data = 0;
	lastSmoothedData = HX711_smoothedData();
	data = lastSmoothedData - tareOffset ;
	float x = (float)data * calFactorRecip;
	return x;
}

long HX711_smoothedData() 
{
	long data = 0;
	long L = 0xFFFFFF;
	long H = 0x00;
	for (uint8_t r = 0; r < (samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE); r++) 
	{
		#if IGN_LOW_SAMPLE
		if (L > dataSampleSet[r]) L = dataSampleSet[r]; // find lowest value
		#endif
		#if IGN_HIGH_SAMPLE
		if (H < dataSampleSet[r]) H = dataSampleSet[r]; // find highest value
		#endif
		data += dataSampleSet[r];
	}
	#if IGN_LOW_SAMPLE 
	data -= L; //remove lowest value
	#endif
	#if IGN_HIGH_SAMPLE 
	data -= H; //remove highest value
	#endif
	//return data;
	return (data >> divBit);

}

void HX711_conversion24bit()  //read 24 bit data, store in dataset and start the next conversion
{
	conversionTime = getTimeInUs() - conversionStartTime;
	conversionStartTime = getTimeInUs();
	unsigned long data = 0;
	uint8_t dout;
	convRslt = 0;
	// if(SCK_DISABLE_INTERRUPTS) noInterrupts();

	for (uint8_t i = 0; i < (24 + gainSetting); i++) 
	{ 	//read 24 bit data + set gain and start next conversion
		GPIO_write(sckGpioNum, HIGH);
		if(SCK_DELAY) sleepForUs(1); // could be required for faster mcu's, set value in config.h
		GPIO_write(sckGpioNum, LOW);
		if (i < (24)) 
		{
			dout = GPIO_read(doutGpioNum);
			data = (data << 1) | dout;
		} else {
			if(SCK_DELAY) sleepForUs(1); // could be required for faster mcu's, set value in config.h
		}
	}
	// if(SCK_DISABLE_INTERRUPTS) interrupts();
	
	/*
	The HX711 output range is min. 0x800000 and max. 0x7FFFFF (the value rolls over).
	In order to convert the range to min. 0x000000 and max. 0xFFFFFF,
	the 24th bit must be changed from 0 to 1 or from 1 to 0.
	*/
	data = data ^ 0x800000; // flip the 24th bit 
	
	if (data > 0xFFFFFF) 
	{
		dataOutOfRange = 1;
		//Serial.println("dataOutOfRange");
	}
	if (reverseVal) {
		data = 0xFFFFFF - data;
	}
	if (readIndex == samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE - 1) 
	{
		readIndex = 0;
	}
	else 
	{
		readIndex++;
	}
	if(data > 0)  
	{
		convRslt++;
		dataSampleSet[readIndex] = (long)data;
		if(doTare) 
		{
			if (tareTimes < DATA_SET) 
			{
				tareTimes++;
			}
			else 
			{
				tareOffset = HX711_smoothedData();
				tareTimes = 0;
				doTare = 0;
				tareStatus = 1;
				convRslt++;
			}
		}
	}
}

//power down the HX711
void HX711_powerDown() 
{
	GPIO_write(sckGpioNum, LOW);
	GPIO_write(sckGpioNum, HIGH);
}

//power up the HX711
void HX711_powerUp() 
{
	GPIO_write(sckGpioNum, LOW);
}

//get the tare offset (raw data value output without the scale "calFactor")
long HX711_getTareOffset() 
{
	return tareOffset;
}

//set new tare offset (raw data value input without the scale "calFactor")
void HX711_setTareOffset(long newoffset)
{
	tareOffset = newoffset;
}

//for testing and debugging:
//returns current value of dataset readIndex
int HX711_getReadIndex()
{
	return readIndex;
}

//for testing and debugging:
//returns latest conversion time in getTimeInMs
float HX711_getConversionTime()
{
	return conversionTime/1000.0;
}

//for testing and debugging:
//returns the HX711 conversions ea seconds based on the latest conversion time. 
//The HX711 can be set to 10SPS or 80SPS. For general use the recommended setting is 10SPS.
float HX711_getSPS()
{
	float sps = 1000000.0/conversionTime;
	return sps;
}

//for testing and debugging:
//returns the tare timeout flag from the last tare operation. 
//0 = no timeout, 1 = timeout
bool HX711_getTareTimeoutFlag() 
{
	return tareTimeoutFlag;
}

void HX711_disableTareTimeout()
{
	tareTimeoutDisable = 1;
}

long HX711_getSettlingTime() 
{
	long st = HX711_getConversionTime() * DATA_SET;
	return st;
}

//overide the number of samples in use
//value is rounded down to the nearest valid value
void HX711_setSamplesInUse(int samples)
{
	int old_value = samplesInUse;
	
	if(samples <= SAMPLES)
	{
		if(samples == 0) //reset to the original value
		{
			divBit = divBitCompiled;
		} 
		else
		{
			samples >>= 1;
			for(divBit = 0; samples != 0; samples >>= 1, divBit++);
		}
		samplesInUse = 1 << divBit;
		
		//replace the value of all samples in use with the last conversion value
		if(samplesInUse != old_value) 
		{
			for (uint8_t r = 0; r < samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE; r++) 
			{
				dataSampleSet[r] = lastSmoothedData;
			}
			readIndex = 0;
		}
	}
}

//returns the current number of samples in use.
int HX711_getSamplesInUse()
{
	return samplesInUse;
}

//resets index for dataset
void HX711_resetSamplesIndex()
{
	readIndex = 0;
}

//Fill the whole dataset up with new conversions, i.e. after a reset/restart (this function is blocking once started)
bool HX711_refreshDataSet()
{
	int s = HX711_getSamplesInUse() + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE; // get number of samples in dataset
	HX711_resetSamplesIndex();
	while ( s > 0 ) {
		HX711_update();
		sleepForUs(1);
		if (GPIO_read(doutGpioNum) == LOW) { // HX711 dout pin is pulled low when a new conversion is ready
			HX711_getData(); // add data to the set and start next conversion
			s--;
		}
	}
	return true;
}

//returns 'true' when the whole dataset has been filled up with conversions, i.e. after a reset/restart.
bool HX711_getDataSetStatus()
{
	bool i = false;
	if (readIndex == samplesInUse + IGN_HIGH_SAMPLE + IGN_LOW_SAMPLE - 1) 
	{
		i = true;
	}
	return i;
}

//returns and sets a new calibration value (calFactor) based on a known mass input
float HX711_getNewCalibration(float known_mass)
{
	float readValue = HX711_getData();
	float exist_calFactor = HX711_getCalFactor();
	float new_calFactor;
	new_calFactor = (readValue * exist_calFactor) / known_mass;
	HX711_setCalFactor(new_calFactor);
    return new_calFactor;
}

//returns 'true' if it takes longer time then 'SIGNAL_TIMEOUT' for the dout pin to go low after a new conversion is started
bool HX711_getSignalTimeoutFlag()
{
	return signalTimeoutFlag;
}

//reverse the output value (flip positive/negative value)
//tare/zero-offset must be re-set after calling this.
void HX711_setReverseOutput() {
	reverseVal = true;
}
