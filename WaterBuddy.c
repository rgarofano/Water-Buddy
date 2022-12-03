
#include "WaterBuddy.h"
#include "WaterBuddy_pinDefines.h"

#include "devices/Scale.h"
#include "devices/RFIDReader.h"

#include <stdlib.h>

static void startServer(void)
{
    system("node server/app.js");
}

static void init(void)
{
    startServer();
    Scale_init(SCALE_CLK_PIN, SCALE_CLK_GPIO, SCALE_DATA_GPIO, SCALE_DATA_GPIO, SCALE_REQ_PIN, SCALE_REQ_GPIO);
    RFIDReader_init(RFID_SPI_PORT_NUM, RFID_SPI_CHIP_SEL, RFID_RST_PIN, RFID_RST_GPIO);
    // TODO: LCD init
}

void WaterBuddy_start(void)
{
    init();
}


