#ifndef USER_H
#define USER_H

#include <stdint.h>

typedef struct {
    uint8_t id;
    char* phoneNumber;
    double waterIntakeGoalLiters;
    double reminderFrequencyHours;
} user_t;

#endif