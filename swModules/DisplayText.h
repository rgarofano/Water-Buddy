#ifndef DISPLAYTEXT_H
#define DISPLAYTEXT_H

void DisplayText_init(void);

void DisplayText_idleMessage(void);

void DisplayText_waitingForUserDataMessage(void);

void DisplayText_registerUserMessage(double goalAmount);

void DisplayText_welcomeExistingUserMessage(double goalAmount, double amountRemaining);

#endif