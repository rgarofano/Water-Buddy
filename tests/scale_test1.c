
/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
 * Settling time (number of samples) and data filtering can be adjusted in the config.h file
 * For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"
 *
 * The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
 * sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
 * If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
 * see example file "Read_1x_load_cell_interrupt_driven.ino".
 *
 * This is an example sketch on how to use this library
 */

#include "../devices/HX711_ADC.h"

#include <stdlib.h>
#include <stdbool.h>

#include "../hwCommon/SystemTools.h"

#define HX711_DOUT_PIN      "p9_11"
#define HX711_SCK_PIN       "p9_13"
#define HX711_DOUT_GPIO_NUM 30
#define HX711_SCK_GPIO_NUM  31

// #if defined(ESP8266)|| defined(ESP32) || defined(AVR)
// #include <EEPROM.h>
// #endif

//pins:
const int HX711_dout = 30; //mcu > HX711 dout pin
const int HX711_sck = 31; //mcu > HX711 sck pin

//HX711 constructor:

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
    // Serial.begin(57600); delay(10);
    // Serial.println();
    // Serial.println("Starting...");
    printf("Starting...\n");

    HX711_begin(HX711_DOUT_PIN, HX711_SCK_PIN, HX711_DOUT_GPIO_NUM, HX711_SCK_GPIO_NUM);
    //HX711_setReverseOutput(); //uncomment to turn a negative output value to positive
    float calibrationValue; // calibration value (see example file "Calibration.ino")
    calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch

    unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
    bool _tare = true; //set this to false if you don't want tare to be performed in the next step
    HX711_start(stabilizingtime, _tare);
    if (HX711_getTareTimeoutFlag()) {
        // Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
        printf("Timeout, check MCU>HX711 wiring and pin designations\n");
        exit(1);
    }
    else {
        HX711_setCalFactor(calibrationValue); // set calibration value (float)
        printf("Startup is complete\n");
    }
}

void loop() {
    static bool newDataReady = false;
    const int serialPrintInterval = 0; //increase value to slow down serial print activity

    // check for new data/start next conversion:
    if (HX711_update()) newDataReady = true;

    // get smoothed value from the dataset:
    if (newDataReady) {
        if (getTimeInMs() > t + serialPrintInterval) {
            float i = HX711_getData();
            printf("Load_cell output val: ");
            printf("%f\n", i);
            newDataReady = 0;
            t = getTimeInMs();
        }
    }

    // receive command from serial terminal, send 't' to initiate tare operation:
    // if (Serial.available() > 0) {
    //   char inByte = Serial.read();
    //   if (inByte == 't') HX711_tareNoDelay();
    // }

    // check if last tare operation is complete:
    if (HX711_getTareStatus() == true) {
        printf("Tare complete\n");
    }

}

int main()
{
    setup();
    while(true) {
        loop();
    }
}