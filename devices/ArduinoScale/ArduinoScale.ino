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

#include <HX711_ADC.h>

#define BITS_PER_INT 32

// Pins:
#define HX711_DOUT_PIN  4 //mcu > HX711 dout pin
#define HX711_SCK_PIN   5 //mcu > HX711 sck pin

#define BBG_REQ_PIN   7
#define BBG_DATA_PIN  8
#define BBG_CLK_PIN   9

#define STABILIZING_TIME_MS 2000
#define TARE_ON_START true

#define CALIBRATION_FACTOR -380.0f // Experimentally found

float weight = 0;

HX711_ADC LoadCell(HX711_DOUT_PIN, HX711_SCK_PIN);

void setup()
{
  Serial.begin(57600);
  delay(10); // Arbitrary
  Serial.println("Starting...");

  pinMode(BBG_REQ_PIN, INPUT);
  pinMode(BBG_DATA_PIN, OUTPUT);
  pinMode(BBG_CLK_PIN, INPUT);

  LoadCell.begin();
  LoadCell.start(STABILIZING_TIME_MS, TARE_ON_START);
  if (LoadCell.getTareTimeoutFlag() || LoadCell.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    exit(1);
  }
  else {
    LoadCell.setCalFactor(1.0);
    Serial.println("Startup complete");
  }
  while (!LoadCell.update());
  // calibrate();
  LoadCell.setCalFactor(CALIBRATION_FACTOR);
} 

void loop()
{
  if (LoadCell.update()) {
    weight = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(weight);
  }
  
  if(digitalRead(BBG_REQ_PIN) == HIGH) {
    exportWeight(weight);
  }
}

void calibrate()
{
  LoadCell.tareNoDelay();
  while(LoadCell.getTareStatus() == false) {
    LoadCell.update();
  }

  Serial.println("Place your known mass on the loadcell.");
  Serial.println("Then send the weight of this mass from serial monitor.");

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

  Serial.print("New calibration value: ");
  Serial.println(newCalibrationValue);

  Serial.println("To re-calibrate, send 'r' from serial monitor.");
  Serial.println("For manual edit of the calibration value, send 'c' from serial monitor.");
}

void exportWeight(float weight)
{
  int weightAsInt = (int)(weight);
  
  for(int i = BITS_PER_INT - 1; i >=0; i--)
  {
    while(digitalRead(BBG_CLK_PIN) != HIGH);
    
    uint8_t bitOfWeight = (weightAsInt >> i) & 1;
    digitalWrite(BBG_DATA_PIN, bitOfWeight);
    
    while(digitalRead(BBG_CLK_PIN) != LOW);
  }
}
