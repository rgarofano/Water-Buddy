
#ifndef WATER_BUDDY_PIN_DEFINES_H
#define WATER_BUDDY_PIN_DEFINES_H

// Scale-Arduino
#define SCALE_REQ_PIN   "p8_08"
#define SCALE_REQ_GPIO  67
#define SCALE_ACK_PIN   "p8_10"
#define SCALE_ACK_GPIO  68
#define SCALE_CLK_PIN   "p8_12"
#define SCALE_CLK_GPIO  44
#define SCALE_DATA_PIN  "p8_14"
#define SCALE_DATA_GPIO 26

// RFID Reader
#define RFID_SPI_PORT_NUM   1
#define RFID_SPI_CHIP_SEL   0
#define RFID_RST_PIN        "p9_15"
#define RFID_RST_GPIO       48

// LCD
#define LCD_I2C_BUS 2
#define LCD_I2C_ADDR 0x27

// Button
#define BUTTON_GPIO_PIN "p9_16"
#define BUTTON_GPIO_NUMBER 51

#endif