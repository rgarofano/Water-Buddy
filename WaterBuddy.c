
#include "WaterBuddy.h"
#include "WaterBuddy_pinDefines.h"

#include "devices/Scale.h"
#include "devices/RFIDReader.h"

#include "swModules/User.h"
#include "swModules/JSON.h"
#include "swModules/HTTP.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define INIT_ARRAY_SIZE 4

/* Thread IDs */
static pthread_t serverTid;

/* User Data */
static int numberUsers = 0;
static int maxNumberUsers;
static user_t* userData;

static void* startServer(void* _arg)
{
    system("node server/app.js");
}

static void initializeUserArray() 
{
    userData = malloc(INIT_ARRAY_SIZE * sizeof(*userData));
    if (userData == NULL) {
        perror("Error allocating memory");
    }

    maxNumberUsers = INIT_ARRAY_SIZE;
}

static void doubleArraySize()
{
    maxNumberUsers *= 2;
    userData = realloc(userData, maxNumberUsers * sizeof(*userData));
}

static void addUser(uint64_t uid)
{
    // get up to date form data from server
    HTTP_sendGetRequest("/data");
    
    user_t newUser;
    newUser.id = uid;
    // parse formData.json to add the remaining
    // fileds to the struct
    JSON_getUserDataFromFile("../formData.json", &newUser);

    numberUsers++;
    if (numberUsers == maxNumberUsers) {
        doubleArraySize();
    }

    userData[numberUsers - 1] = newUser;
}

static void init(void)
{
    pthread_create(&serverTid, NULL, startServer, NULL);

    Scale_init( SCALE_REQ_PIN,
                SCALE_REQ_GPIO,
                SCALE_ACK_PIN,
                SCALE_ACK_GPIO,
                SCALE_CLK_PIN,
                SCALE_CLK_GPIO,
                SCALE_DATA_PIN,
                SCALE_DATA_GPIO);

    RFIDReader_init(RFID_SPI_PORT_NUM, RFID_SPI_CHIP_SEL, RFID_RST_PIN, RFID_RST_GPIO);
    
    // TODO: LCD init

    initializeUserArray();
}

void WaterBuddy_start(void)
{
    init();
}