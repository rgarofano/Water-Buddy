#include "../devices/LCDDisplay.h"

#include "../WaterBuddy_pinDefines.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define  FIRST_LINE 0
#define SECOND_LINE 1
#define  THIRD_LINE 2
#define FOURTH_LINE 3

#define CHARS_PER_LINE 20

#define PROPORTION_OF_GOAL 0.8
#define GOAL_THRESHOLD(goal, proportion) (proportion * goal)

#define GOAL_TEMPLATE      "  Goal: %.1lf Liters  "
#define REMAINING_TEMPLATE "%.1lf Liters Remaining"

static LCD* lcd;

enum LAST_MESSAGE {
    NO_MESSAGE = -1,
    IDLE = 0,
    REGISTER = 1,
    EXISTING_USER = 2,
};

static enum LAST_MESSAGE lastMessage = NO_MESSAGE;

void DisplayText_init(void)
{
    lcd = LCDDisplay_init(LCD_I2C_BUS, LCD_I2C_ADDR);
}

void DisplayText_idleMessage(void)
{
    if (lastMessage == IDLE) {
        return;
    }

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "   Water Buddy :)   ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, "Its time to drink up");

    lastMessage = IDLE;
}

void DisplayText_registerUserMessage(double goalAmount)
{
    if (lastMessage == REGISTER) {
        return;
    }

    char goalMessage[CHARS_PER_LINE + 1];
    snprintf(goalMessage, CHARS_PER_LINE + 1, GOAL_TEMPLATE, goalAmount);
    goalMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "     Welcome :)     ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, " I see hydration in ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "   in your future   ");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, goalMessage);

    lastMessage = REGISTER;
}

void DisplayText_welcomeExistingUserMessage(double goalAmount, double amountRemaining)
{

    if (lastMessage == EXISTING_USER) {
        return;
    }

    bool closeToGoal = 
        amountRemaining >= GOAL_THRESHOLD(goalAmount, PROPORTION_OF_GOAL);
    
    char* thirdLineMessage = closeToGoal ? "  You're so close!  "
                                         : "  Keep on drinking  ";

    char amountRemainingMessage[CHARS_PER_LINE + 1];
    snprintf(amountRemainingMessage, CHARS_PER_LINE + 1, REMAINING_TEMPLATE, amountRemaining);
    amountRemainingMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "   Welcome Back :)  ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "  Did you miss me?  ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, thirdLineMessage);
    LCDDisplay_writeLine(lcd, FOURTH_LINE, amountRemainingMessage);

    lastMessage = EXISTING_USER;
}