/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
   This example file shows how to calibrate the load cell and optionally store the calibration
   value in EEPROM, and also how to change the value manually.
   The result value can then later be included in your project sketch or fetched from EEPROM.

   To implement calibration in your project sketch the simplified procedure is as follow:
       LoadCell.tare();
       //place known mass
       LoadCell.refreshDataSet();
       float newCalibrationValue = LoadCell.getNewCalibration(known_mass);
*/
#include <Arduino.h>
#include <HX711_ADC.h>

//pins:
const int HX711_doutPin = 4; //mcu > HX711 dout pin
const int HX711_sckPin = 5; //mcu > HX711 sck pin

const int BBG_reqPin = 7;
const int BBG_dataPin = 8;
const int BBG_clkPin = 9;

//HX711 constructor:
HX711_ADC LoadCell(HX711_doutPin, HX711_sckPin);

unsigned long timeOfPreviousGetDataMs = 0;
float weight = 0;

#define BITS_PER_INT 32

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");

  pinMode(BBG_reqPin, INPUT);
  pinMode(BBG_dataPin, OUTPUT);
  pinMode(BBG_clkPin, INPUT);

  LoadCell.begin();
  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    exit(1);
  }
  else {
    LoadCell.setCalFactor(1.0); // user set calibration value (float), initial value 1.0 may be used for this sketch
    Serial.println("Startup complete");
  }
  while (!LoadCell.update());
  calibrate(); //start calibration procedure
}

void loop() {
  static boolean newDataReady = 0;

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    weight = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(weight);
    newDataReady = 0;
    timeOfPreviousGetDataMs = millis();
  }
  
  if(digitalRead(BBG_reqPin) == HIGH) {
    sendWeight(weight);
  }
}

void calibrate() {
  LoadCell.tareNoDelay();
  while(LoadCell.getTareStatus() == false) {
    LoadCell.update();
  }

  Serial.println("Place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass (i.e. 100.0) from serial monitor.");

  float known_mass = 0;
  bool _resume = false;
  while (_resume == false) {
    LoadCell.update();
    if (Serial.available() > 0) {
      known_mass = Serial.parseFloat();
      if (known_mass != 0) {
        Serial.print("Known mass is: ");
        Serial.println(known_mass);
        _resume = true;
      }
    }
  }

  LoadCell.refreshDataSet(); //refresh the dataset to be sure that the known mass is measured correct
  float newCalibrationValue = LoadCell.getNewCalibration(known_mass); //get the new calibration value

  Serial.print("New calibration value has been set to: ");
  Serial.print(newCalibrationValue);

  Serial.println("End calibration");
  Serial.println("***");
  Serial.println("To re-calibrate, send 'r' from serial monitor.");
  Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
  Serial.println("***");
}

void sendWeight(float weight)
{
  int weightAsInt = (int)(weight);
  
  for(int i = BITS_PER_INT - 1; i >=0; i--)
  {
    while(digitalRead(BBG_clkPin) == LOW);
    
    int bit = (weightAsInt >> i) & 1;
    digitalWrite(BBG_dataPin, bit);
    
    while(digitalRead(BBG_clkPin) == HIGH);
  }
}
