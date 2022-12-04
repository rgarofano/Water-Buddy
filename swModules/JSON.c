#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "User.h"

#define BUFF_SIZE 1024
#define KEY_PHONE_NUMBER "phone"
#define KEY_REMINDER_FREQUENCY "frequency"
#define KEY_DAILY_INTAKE "goal"
#define NUM_CHARS_PHONE_NUMBER 10
#define NUM_CHARS_DAILY_INTAKE 3
#define NUM_CHARS_REMINDER_FREQUENCY 3

static char* parseValueFromKey(char* json, char* key, int size)
{
    char* keyStart = strstr(json, key);
    int charIndex = 0;
    // get the index of the first character of the value
    // stored by this key
    for (int i = 0; i < strlen(key) + 3; i++) {
        charIndex++;
    }
    

    char* buff = malloc((size + 1) * sizeof(*buff));
    int buffIndex = 0;
    while (atoi("\"") != (int)keyStart[charIndex] && buffIndex < size) {
        buff[buffIndex++] = keyStart[charIndex++];
    }
    buff[size] = 0;

    return buff;
}

static double getDoubleFromJson(char* json, char* key, int expectedNumChars)
{
    char* parsedString = parseValueFromKey(json, key, expectedNumChars);
    double parsedValueAsDouble = strtod(parsedString, NULL);
    free(parsedString);
    return parsedValueAsDouble;
}

static char* getStringFromJson(char* json, char* key, int expectedNumChars)
{
    return parseValueFromKey(json, key, expectedNumChars);
}

void JSON_getUserDataFromFile(char* filePath, user_t* userData)
{
    FILE* pFile = fopen(filePath, "r");
    if (pFile == NULL) {
        perror("Error Opening File");
    }

    char buffer[BUFF_SIZE];
    fread(buffer, BUFF_SIZE, sizeof(char), pFile);
    fclose(pFile);
    
    char* phoneNumber = 
        getStringFromJson(buffer, KEY_PHONE_NUMBER, NUM_CHARS_PHONE_NUMBER);
    userData->phoneNumber = phoneNumber;

    double waterIntake = 
        getDoubleFromJson(buffer, KEY_DAILY_INTAKE, NUM_CHARS_DAILY_INTAKE);
    userData->waterIntakeGoalLiters = waterIntake;

    double reminderFrequency = 
        getDoubleFromJson(buffer, KEY_REMINDER_FREQUENCY, NUM_CHARS_REMINDER_FREQUENCY);
    userData->reminderFrequencyHours = reminderFrequency;
}