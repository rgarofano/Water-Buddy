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

#define BBG_REQ_PIN   11
#define BBG_ACK_PIN   10
#define BBG_CLK_PIN   9
#define BBG_DATA_PIN  8

#define STABILIZING_TIME_MS 2000
#define TARE_ON_START true

#define CALIBRATION_FACTOR_GRAMS -380.0f // Experimentally found
#define NUM_SAMPLES_IN_USE 4

#define SERIAL_BAUD_RATE 57600

float weightGrams = 0;

HX711_ADC LoadCell(HX711_DOUT_PIN, HX711_SCK_PIN);

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("Starting...");

  pinMode(HX711_DOUT_PIN, INPUT);
  pinMode(HX711_SCK_PIN, OUTPUT);

  pinMode(BBG_REQ_PIN, INPUT);
  pinMode(BBG_ACK_PIN, OUTPUT);
  pinMode(BBG_CLK_PIN, INPUT);
  pinMode(BBG_DATA_PIN, OUTPUT);

  digitalWrite(BBG_ACK_PIN, LOW);
  digitalWrite(BBG_DATA_PIN, LOW);

  LoadCell.begin();
  LoadCell.setSamplesInUse(NUM_SAMPLES_IN_USE);
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
  LoadCell.setCalFactor(CALIBRATION_FACTOR_GRAMS);
} 

void loop()
{
  if (LoadCell.update()) {
    weightGrams = LoadCell.getData();
    Serial.print("Load_cell output val: ");
    Serial.println(weightGrams);
  }
  
  if(digitalRead(BBG_REQ_PIN) == HIGH) {
    // Acknowledge
    digitalWrite(BBG_ACK_PIN, HIGH);
    exportWeight();
    digitalWrite(BBG_ACK_PIN, LOW);
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
}

void exportWeight(void)
{
  int weightAsInt = (int)(weightGrams);
  
  for(int i = BITS_PER_INT - 1; i >=0; i--)
  {
    while(digitalRead(BBG_CLK_PIN) != HIGH);
    
    uint8_t bitOfWeight = (weightAsInt >> i) & 1;
    digitalWrite(BBG_DATA_PIN, bitOfWeight);
    
    while(digitalRead(BBG_CLK_PIN) != LOW);
  }
}
