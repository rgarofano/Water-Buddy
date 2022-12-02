
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../hwCommon/SystemTools.h"
#include "../hwCommon/GPIO.h"

#include "../devices/RFIDReader.h"

#define SPI_PORT_NUM 1
#define SPI_CHIP_SEL 0
#define RST_PIN "p9_11"
#define RST_GPIO_NUM 30

static int check(bool condition) {
    if(condition) {
        printf("====================================================== PASSED\n");
        return 0;
    }
    else {
        printf("============================================================= FAILED\n");
        return -1;
    }
}

static void testFIFO(void)
{
    printf("\ntestFIFO\n");
    RFIDReader_init(SPI_PORT_NUM, SPI_CHIP_SEL, RST_PIN, RST_GPIO_NUM);

    uint8_t size = 6;

    uint8_t *writeBuffer = malloc(size);
    uint8_t *readBuffer = malloc(size);

    for(int i = 0; i < size; i++) {
        writeBuffer[i] = 2 * i + 3;
    }

    RFIDReader_writeFIFO(writeBuffer, size);
    RFIDReader_readFIFO(readBuffer, size);

    bool passCond = true;
    printf("Write Buffer: ");
    for(int i = 0; i < size; i++) {
        printf("%d ", writeBuffer[i]);
    }
    printf("\nRead Buffer: ");
    for(int i = 0; i < size; i++) {
        printf("%d ", readBuffer[i]);
        if(writeBuffer[i] != readBuffer[i]) {
            passCond = false;
        }
    }
    printf("\n");

    check(passCond);
}

static void testRequestUID(void)
{
    printf("\ntestGetUID\n");

    RFIDReader_init(SPI_PORT_NUM, SPI_CHIP_SEL, RST_PIN, RST_GPIO_NUM);

    uint64_t uid = 0;

    int status = RFIDReader_sendPiccCommand(PICC_REQA);
    if(status < 0) {
        printf("Error: PICC Request Failed\n");
    }

    status = RFIDReader_selectPICCAndGetUID(&uid);
    if(status < 0) {
        printf("Error: Get UID Failed\n");
    }

    printf("UID: %llx\n", uid);

    check(status == STATUS_OK);
}

static void testGetImmediateUID(void)
{
    printf("\ntestGetImmediateUID\n");

    RFIDReader_init(SPI_PORT_NUM, SPI_CHIP_SEL, RST_PIN, RST_GPIO_NUM);

    uint64_t uid = 0;

    int status = RFIDReader_getImmediateUID(&uid);
    if(status < 0) {
        printf("Error: status %d\n", status);
    }

    printf("UID: %llx\n", uid);

    check(status == STATUS_OK);
}

static void runTestSuite(void)
{
    printf("RFIDReader Test Suite\n");

    #define NUM_TESTS 3
    void (*testFunctions[NUM_TESTS])(void);

    testFunctions[0] = testFIFO;
    testFunctions[1] = testRequestUID;
    testFunctions[2] = testGetImmediateUID;

    for(int i = 0; i < NUM_TESTS; i++) {
        sleepForMs(100);
        (*testFunctions[i])();
        GPIO_write(RST_GPIO_NUM, 0);
    }
}

int main()
{
    runTestSuite();
}