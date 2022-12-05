#ifndef DISPLAYTEXT_H
#define DISPLAYTEXT_H

void DisplayText_init(int bus);

void DisplayText_idleMessage(void);

void DisplayText_waitingForUserDataMessage(void);

void DisplayText_registerUserMessage(double goalAmount);

void DisplayText_welcomeExistingUserMessage(double goalAmount, double amountRemaining);

void DisplayText_fillingUpMessage(int currentVolumeML);

#endif