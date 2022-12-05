
#include "WaterBuddy.h"
#include "WaterBuddy_pinDefines.h"

#include "hwCommon/SystemTools.h"

#include "devices/Scale.h"
#include "devices/RFIDReader.h"
#include "devices/Button.h"
#include "devices/LCDDisplay.h"

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

#define ML_PER_L 1000

/* Mutex */
static pthread_mutex_t userDataMutex = PTHREAD_MUTEX_INITIALIZER;

/* Thread Variables */
static pthread_t serverTid;
static pthread_t waterDispenserTid;
static pthread_t schedulerTid;
static pthread_t sendReminderTid;

#define SERVER_STARTUP_TIME_ESTIMATE_MS 10000

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

// Returns whether it was successful or not (true on successful, false on fail)
static bool addUser(uint64_t uid)
{
    // get up to date form data from server
    HTTP_sendGetRequest("/data");
    
    user_t newUser;
    newUser.uid = uid;
    newUser.lastReminderTimeHours = getTimeInHours();
    // parse formData.json to add the remaining
    // fileds to the struct
    bool status = JSON_getUserDataFromFile("formData.json", &newUser);
    if(status == false) {
        return false;
    }

    pthread_mutex_lock(&userDataMutex);
    {
        numberUsers++;
        if (numberUsers == maxNumberUsers) {
            doubleArraySize();
        }

        userData[numberUsers - 1] = newUser;
    }
    pthread_mutex_unlock(&userDataMutex);

    return true;
}

static int getIndexOfUser(uint64_t targetUid)
{
    int index = USER_NOT_FOUND;

    pthread_mutex_lock(&userDataMutex);
    {
        for (int i = 0; i < numberUsers; i++) {
            user_t user = userData[i];
            if (user.uid == targetUid) {
                index = i;
                break;
            }
        }
    }
    pthread_mutex_unlock(&userDataMutex);

    return index;
}

static void* startServer(void* _arg)
{
    printf("Starting Server...\n");
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
        // TODO: LCD welcome screen
        printf("Welcome to WaterBuddy :)\n");

        uint64_t uid = 0;
        enum MFRC522_StatusCode status = RFIDReader_requestPiccAndGetUID(&uid);
        
        if (status != STATUS_OK) {
            sleepForMs(HARDWARE_CHECK_DELAY_MS);
            continue;
        }

        int userIndex = getIndexOfUser(uid);
        bool userIsNew = (userIndex == USER_NOT_FOUND);

        // Add user if uid not registered
        if (userIsNew) {
            printf("Attempt to create new user...\n");
            bool status = addUser(uid);
            if(status == false) {
                printf("Create User Failed: No new user data\n");
                continue;
            }

            userIndex = numberUsers - 1;
            printf("Create User Success! UID: %llx, phone #: %s\n", userData[userIndex].uid, userData[userIndex].phoneNumber);
        }
        else {
            // TODO: LCD Welcome Existing User
            printf("Welcome existing user! UID: %llx, phone #: %s\n", userData[userIndex].uid, userData[userIndex].phoneNumber);
        }

        while(RFIDReader_getActivePiccUID(&uid) == STATUS_OK) {

            // Wait for button Press
            if(!userIsNew) {
                // TODO: LCD Welcome Existing User
            }

            printf("Start Filling\n");

            while (!Button_isPressed(DISPENSE_BUTTON_GPIO) && RFIDReader_getActivePiccUID(&uid) == STATUS_OK) {
                sleepForMs(HARDWARE_CHECK_DELAY_MS);
            }

            if(RFIDReader_getActivePiccUID(&uid) != STATUS_OK) {
                sleepForMs(HARDWARE_CHECK_DELAY_MS);
                break;
            }

            int initialWeightGrams = Scale_getWeightGrams();
            int dispensedWeightGrams = 0;

            while(Button_isPressed(DISPENSE_BUTTON_GPIO)) {
                dispensedWeightGrams = Scale_getWeightGrams() - initialWeightGrams;
                // TODO: LCD fill progress screen
                printf("Filled: %d\n", dispensedWeightGrams);
            }
            
            // Calculate the dispensed volume and update goal. Water's mass to volume conversion is about 1
            int dispensedVolumeMl = dispensedWeightGrams;
            double dispensedVolumeL = (double)dispensedVolumeMl / (double)ML_PER_L;

            userData[userIndex].waterIntakeProgressLitres += dispensedVolumeL;
            if(userData[userIndex].waterIntakeProgressLitres > userData[userIndex].waterIntakeGoalLiters) {
                userData[userIndex].waterIntakeGoalLiters = userData[userIndex].waterIntakeProgressLitres;
                // TODO: LCD goal reached screen
                printf("Goal Reached! %f / %f L\n",
                    userData[userIndex].waterIntakeProgressLitres,
                    userData[userIndex].waterIntakeGoalLiters
                );
            }
            else {
                // TODO: LCD display post dispense screen
                printf("Goal Progress: %f / %f L\n",
                    userData[userIndex].waterIntakeProgressLitres,
                    userData[userIndex].waterIntakeGoalLiters
                );
            }
        }

        userIsNew = false;
        sleepForMs(HARDWARE_CHECK_DELAY_MS);
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
    
    // LCD init
    LCDDisplay_init(LCD_I2C_BUS, LCD_I2C_ADDR);

    // Button/Switch Init
    Button_init(DISPENSE_BUTTON_PIN, DISPENSE_BUTTON_GPIO);

    // SW module initialization
    pthread_create(&serverTid, NULL, startServer, NULL);
    sleepForMs(SERVER_STARTUP_TIME_ESTIMATE_MS);
    pthread_create(&waterDispenserTid, NULL, waterDispenser, NULL);
    pthread_create(&schedulerTid, NULL, scheduleReminders, NULL);

    initializeUserArray();
}

void WaterBuddy_start(void)
{
    init();
    pthread_join(waterDispenserTid, NULL);
}