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
#define DISPENSED_AMOUNT_SIZE 8
#define DISPENSED_AMOUNT_COLUMN 13

#define PROPORTION_OF_GOAL 0.2
#define GOAL_THRESHOLD(goal, proportion) (proportion * goal)

#define GOAL_TEMPLATE      "  Goal: %.1lf Liters  "
#define REMAINING_TEMPLATE "%.1lf Liters Remaining"
#define AMOUNT_TEMPLATE    "%4d ml"

#define NUM_JOKES 5

typedef struct {
    char* firstLineMessage;
    char* secondLineMessage;
    char* thirdLineMessage;
    char* fourthLineMessage;
} joke_t;

static int jokeIndex = 0;

static joke_t jokes[NUM_JOKES] = 
{
    {
        "       Always       ",
        "        Keep        ",
        "       Things       ",
        "       Tide-y       "
    
    },
    {
        "This is a joke about",
        "    a submarine     ",
        "     let that       ",
        "     sink in..      "
    
    },
    {
        " What do you call a ",
        "  group of singing  ",
        "       whales?      ",
        "    An orca-stra    "
    },
    {
        "  I never believed  ",
        " water could freeze ",
        "    but now icy     ",
        "                    "
    },
    {
        "What do you call two",
        "  days in a row of  ",
        " rain in Vancouver? ",
        "    The weekend     "
    
    }
}; 

static LCD* lcd;

enum LAST_MESSAGE {
    NO_MESSAGE = -1,
    IDLE = 0,
    REGISTER = 1,
    NEW_USER = 2,
    EXISTING_USER = 3,
    FILLING = 4
};

static enum LAST_MESSAGE lastMessage = NO_MESSAGE;

void DisplayText_init(int bus)
{
    lcd = LCDDisplay_init(bus);
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

void DisplayText_waitingForUserDataMessage(void)
{   
    if (lastMessage != REGISTER) {
        LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
        LCDDisplay_writeLine(lcd, SECOND_LINE, " Register New User  ");
        LCDDisplay_writeLine(lcd, FOURTH_LINE, "--------------------");
    }

    LCDDisplay_writeLine(lcd, THIRD_LINE, "                    ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, "       .            ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, "       ..           ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, "       ...          ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, "       ....         ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, "       .....        ");

    lastMessage = REGISTER;
}

void DisplayText_registerUserMessage(double goalAmount)
{
    if (lastMessage == NEW_USER) {
        return;
    }

    char goalMessage[CHARS_PER_LINE + 1];
    snprintf(goalMessage, CHARS_PER_LINE + 1, GOAL_TEMPLATE, goalAmount);
    goalMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "     Welcome :)     ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "  I see hydration   ");
    LCDDisplay_writeLine(lcd, THIRD_LINE,  "   in your future   ");
    LCDDisplay_writeLine(lcd, FOURTH_LINE, goalMessage);

    lastMessage = NEW_USER;
}

void DisplayText_welcomeExistingUserMessage(double goalAmount, double amountRemaining)
{

    if (lastMessage == EXISTING_USER) {
        return;
    }

    bool closeToGoal = 
        amountRemaining <= GOAL_THRESHOLD(goalAmount, PROPORTION_OF_GOAL);
    
    bool goalReached = ((int)(amountRemaining * 100.0) == 0);

    char* thirdLineMessage =    goalReached ? "Congrats you did it!" :
                                closeToGoal ? "  You're so close!  " :
                                "  Keep on drinking  ";


    char amountRemainingMessage[CHARS_PER_LINE + 1];
    snprintf(amountRemainingMessage, CHARS_PER_LINE + 1, REMAINING_TEMPLATE, amountRemaining);
    amountRemainingMessage[CHARS_PER_LINE] = 0;

    LCDDisplay_writeLine(lcd, FIRST_LINE,  "   Welcome Back :)  ");
    LCDDisplay_writeLine(lcd, SECOND_LINE, "  Did you miss me?  ");
    LCDDisplay_writeLine(lcd, THIRD_LINE, thirdLineMessage);
    LCDDisplay_writeLine(lcd, FOURTH_LINE, amountRemainingMessage);

    lastMessage = EXISTING_USER;
}

void DisplayText_fillingUpMessage(int currentVolumeML)
{
    if (lastMessage != FILLING) {
        LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
        LCDDisplay_writeLine(lcd, SECOND_LINE, "    Filling Up...   ");
        LCDDisplay_writeLine(lcd, THIRD_LINE,  "  Dispensed: ");
        LCDDisplay_writeLine(lcd, FOURTH_LINE, "--------------------");
    }

    char amountDispensed[DISPENSED_AMOUNT_SIZE];
    snprintf(amountDispensed, DISPENSED_AMOUNT_SIZE, AMOUNT_TEMPLATE, currentVolumeML);
    amountDispensed[DISPENSED_AMOUNT_SIZE - 1] = 0;

    LCDDisplay_mvWrite(lcd, DISPENSED_AMOUNT_COLUMN, THIRD_LINE, amountDispensed);

    lastMessage = FILLING;
}

void DisplayText_postDispenseMessage(int amountRemainingML)
{
    bool goalReached = (amountRemainingML == 0);

    if (goalReached) {
        LCDDisplay_writeLine(lcd, FIRST_LINE,  "--------------------");
        LCDDisplay_writeLine(lcd, SECOND_LINE, "Woohoo! you made it!");
        LCDDisplay_writeLine(lcd, THIRD_LINE,  "See you tomorrow :) ");
        LCDDisplay_writeLine(lcd, FOURTH_LINE, "--------------------");

    } else {
        LCDDisplay_writeLine(lcd, FIRST_LINE, jokes[jokeIndex].firstLineMessage);
        LCDDisplay_writeLine(lcd, SECOND_LINE, jokes[jokeIndex].secondLineMessage);
        LCDDisplay_writeLine(lcd, THIRD_LINE, jokes[jokeIndex].thirdLineMessage);
        LCDDisplay_writeLine(lcd, FOURTH_LINE, jokes[jokeIndex].fourthLineMessage);

        jokeIndex = (jokeIndex + 1) % NUM_JOKES;
    }
}