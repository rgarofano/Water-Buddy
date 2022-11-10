
#include <stdio.h>
#include <stdlib.h>

#include "SystemTools.h"

#include "A2D.h"

int A2D_read(int analogChannel)
{
    const int ANALOG_FILE_NAME_MAX_SIZE = 64;
    char analogValueFileName[ANALOG_FILE_NAME_MAX_SIZE];
    sprintf(analogValueFileName, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", analogChannel);

    FILE* analogValueFile = fopenWithCheck(analogValueFileName, "r");

    int a2DReading = 0;
    int itemsRead = fscanf(analogValueFile, "%d", &a2DReading);
    if(itemsRead <= 0) {
        printf("Error: Unable to read values from voltage input file.\n");
        exit(1);
    }

    fclose(analogValueFile);

    return a2DReading;
}

float A2D_readVoltage(int analogChannel)
{
    float a2dValue = (float)A2D_read(analogChannel);
    return ((a2dValue / MAX_A2D_VALUE) * MAX_AIN_VOLTAGE);
}
