#include "../devices/LCDDisplay.h"

#include "WaterBuddy_pinDefines.h"

#include <stdbool.h>

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

void DisplayText_init(void) {
    lcd = LCDDisplay_init(LCD_I2C_BUS, LCD_I2C_ADDR);
}

void DisplayText_idleMessage(void)
{
    LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "   Water Buddy :)   ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "--------------------");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, "its time to drink up");
}

void DisplayText_registerUserMessage(double goalAmount)
{
    char goalMessage[CHARS_PER_LINE + 1];
    snprintf(goalMessage, CHARS_PER_LINE, GOAL_TEMPLATE, goalAmount);
    goalMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "     Welcome :)     ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, " I see hydration in ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "   in your future   ");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, goalMessage);
}

void DisplayText_welcomeExistingUserMessage(double goalAmount, double amountRemaining)
{
    bool closeToGoal = 
        amountRemaining >= GOAL_THRESHOLD(goalAmount, PROPORTION_OF_GOAL);
    
    char* thirdLineMessage = closeToGoal ? "  You're so close!  "
                                         : "  Keep on drinking  ";

    char amountRemainingMessage[CHARS_PER_LINE + 1];
    snprintf(amountRemainingMessage, CHARS_PER_LINE, REMAINING_TEMPLATE, amountRemaining);
    amountRemainingMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "   Welcome Back :)  ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "  Did you miss me?  ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, thirdLineMessage);
    LCDDisplay_writeLine(lcd, FOURTH_LINE, amountRemainingMessage);

}