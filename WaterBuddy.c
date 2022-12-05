
#include "WaterBuddy.h"
#include "WaterBuddy_pinDefines.h"

#include "devices/Scale.h"
#include "devices/RFIDReader.h"
#include "devices/Button.h"

#include "hwCommon/SystemTools.h"

#include "swModules/User.h"
#include "swModules/JSON.h"
#include "swModules/HTTP.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#define INIT_ARRAY_SIZE 4
#define HARDWARE_CHECK_DELAY_MS 100
#define USER_NOT_FOUND -1
#define REMINDER_CHECK_DELAY_MS 60000
#define SMS_ROUTE_LENGTH 15
#define SMS_ROUTE_TEMPLATE "/sms/%s"

/* Mutex */
static pthread_mutex_t userDataMutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread IDs */
static pthread_t serverTid;
static pthread_t waterDispenserTid;
static pthread_t schedulerTid;
static pthread_t sendReminderTid;

/* User Data */
static int numberUsers = 0;
static int maxNumberUsers;
static user_t* userData;

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
    newUser.uid = uid;
    newUser.lastReminderTimeHours = getTimeInHours();
    // parse formData.json to add the remaining
    // fileds to the struct
    JSON_getUserDataFromFile("formData.json", &newUser);

    pthread_mutex_lock(&userDataMutex);
    {
        numberUsers++;
        if (numberUsers == maxNumberUsers) {
            doubleArraySize();
        }
    }
    pthread_mutex_unlock(&userDataMutex);

    userData[numberUsers - 1] = newUser;
}

static int getIndexOfUser(uint64_t targetUid)
{
    pthread_mutex_lock(&userDataMutex);
    {
        for (int i = 0; i < numberUsers; i++) {
            user_t user = userData[i];
            if (user.uid == targetUid) {
                return i;
            }
        }
    }
    pthread_mutex_unlock(&userDataMutex);

    return USER_NOT_FOUND;
}

static void* startServer(void* _arg)
{
    system("sudo node server/app.js");

    pthread_exit(NULL);
}

static void* sendReminder(void* phoneNumber)
{
    char* phone = (char*)phoneNumber;
    char buffer[SMS_ROUTE_LENGTH + 1];
    snprintf(buffer, SMS_ROUTE_LENGTH, SMS_ROUTE_TEMPLATE, phone);
    buffer[SMS_ROUTE_LENGTH] = 0;
    HTTP_sendPostRequest(buffer);
    pthread_exit(NULL);
}

static void* scheduleReminders(void* _arg)
{
    while (true) {
        pthread_mutex_lock(&userDataMutex);
        {
            for (int i = 0; i < numberUsers; i++) {
                double lastReminderTime = 
                    userData[i].lastReminderTimeHours;

                double timeSinceLastReminder =
                    getTimeInHours() - lastReminderTime;

                double timeBetweenReminders = 
                    userData[i].reminderFrequencyHours;       
                
                if (timeSinceLastReminder >= timeBetweenReminders) {
                    pthread_create(&sendReminderTid, NULL, sendReminder, userData[i].phoneNumber);
                    userData[i].lastReminderTimeHours = getTimeInHours();
                }
            }
        }
        pthread_mutex_unlock(&userDataMutex);

        sleepForMs(REMINDER_CHECK_DELAY_MS);
    }
    pthread_exit(NULL);
}

static void* waterDispenser(void* _arg)
{
    while (true) {
        uint64_t uid = 0;
        enum MFRC522_StatusCode status = RFIDReader_getImmediateUID(&uid);
        
        if (status == STATUS_TIMEOUT) {
            sleepForMs(HARDWARE_CHECK_DELAY_MS);
            continue;
        }

        int userIndex = getIndexOfUser(uid);
        if (userIndex == USER_NOT_FOUND) {
            addUser(uid);
            userIndex = numberUsers - 1;
        }

        printf("Waiting for button\n");
        while (!Button_isPressed(BUTTON_GPIO_NUMBER)) {
            sleepForMs(HARDWARE_CHECK_DELAY_MS);
        }

        // scale stuff goes here
    }

    pthread_exit(NULL);
}

static void init(void)
{
    // HW module initialization
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

    Button_init(BUTTON_GPIO_PIN, BUTTON_GPIO_NUMBER);

    // SW module initialization
    pthread_create(&serverTid, NULL, startServer, NULL);
    pthread_create(&waterDispenserTid, NULL, waterDispenser, NULL);
    pthread_create(&schedulerTid, NULL, scheduleReminders, NULL);

    initializeUserArray();
}

void WaterBuddy_start(void)
{
    init();
    pthread_join(waterDispenserTid, NULL);
}