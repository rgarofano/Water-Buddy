
/**
 * Scale data to BeagleBone driver code
 * Reads the HX711 load cell amp using a pre-existing library, references linked below
 * Sends data to BeagleBone using an improvised serial protocol
 * 
 * This file was also based on an example given in the HX711 Library
 * 
 * HX711 Library:
 * https://github.com/olkal/HX711_ADC
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
