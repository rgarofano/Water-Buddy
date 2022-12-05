#ifndef USER_H
#define USER_H

#include <stdint.h>

typedef struct {
    uint64_t uid;
    char* phoneNumber;
    double waterIntakeGoalLiters;
    double reminderFrequencyHours;
    double lastReminderTimeHours;
} user_t;

#endif