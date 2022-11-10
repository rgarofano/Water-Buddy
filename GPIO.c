
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "SystemTools.h"

#include "GPIO.h"



// GPIO
// #define SLEEP_TIME_AFTER_EXPORT_MS 1000
// static void exportGpioNum(const int gpioNum)
// {
//     FILE *gpioExportFile = fopenWithCheck("/sys/class/gpio/export", "w");
//     fprintf(gpioExportFile, "%d", gpioNum);
//     fclose(gpioExportFile);

//     // Must sleep for a short time while the necessary directories for the specified gpioNum are created
//     sleepForMs(SLEEP_TIME_AFTER_EXPORT_MS);
// }

void configPinForGPIO(const char* pin, const int gpioNum, const char* gpioDirection)
{
    configPin(pin, "gpio");

    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioDirectionFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioDirectionFileName, "/sys/class/gpio/gpio%d/direction", gpioNum);

    FILE *gpioDirectionFile = fopenWithCheck(gpioDirectionFileName, "w");

    fprintf(gpioDirectionFile, gpioDirection);

    fclose(gpioDirectionFile);
}

int gpioRead(int gpioNum)
{
    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioValueFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioValueFileName, "/sys/class/gpio/gpio%d/value", gpioNum);

    FILE* gpioValueFile = fopenWithCheck(gpioValueFileName, "r");

    const int VAL_BUFFER_MAX_SIZE = 16;
    char valueBuffer[VAL_BUFFER_MAX_SIZE];
    fgets(valueBuffer, VAL_BUFFER_MAX_SIZE, gpioValueFile);

    fclose(gpioValueFile);

    int value = atoi(valueBuffer);

    return value;
}

void gpioWrite(int gpioNum, int value)
{
    const int GPIO_FILE_NAME_MAX_SIZE = 64;
    char gpioValueFileName[GPIO_FILE_NAME_MAX_SIZE];
    sprintf(gpioValueFileName, "/sys/class/gpio/gpio%d/value", gpioNum);

    FILE* gpioValueFile = fopenWithCheck(gpioValueFileName, "w");

    fprintf(gpioValueFile, "%d", value);

    fclose(gpioValueFile);
}



// LED
void setLEDAttribute(const int LEDNum, const char* attribute, const char* value)
{
    if(attribute == NULL || value == NULL) {
        printf("ERROR: NULL arguments in setLEDAttribute() for LED%d", LEDNum);
        exit(1);
    }

    const int LED_FILE_NAME_MAX_SIZE = 50;
    char ledAttFileName[LED_FILE_NAME_MAX_SIZE];
    sprintf(ledAttFileName, "/sys/class/leds/beaglebone:green:usr%d/%s", LEDNum, attribute);

    FILE* ledAttFile = fopenWithCheck(ledAttFileName, "w");

    fprintf(ledAttFile, "%s", value);

    fclose(ledAttFile);
}
