
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "swModules/User.h"
#include "swModules/JSON.h"

static void startServer(void)
{
    system("node server/app.js");
}

static void testJson(void)
{
    user_t userData;
    userData.id = 0x33;
    JSON_getUserDataFromFile("formData.json", &userData);
    printf("User Data:\nid: %d\nphone: %s\ngoal (Liters): %lf\nremind me every %lf hours\n",
            userData.id,
            userData.phoneNumber,
            userData.waterIntakeGoalLiters,
            userData.reminderFrequencyHours);
    free(userData.phoneNumber);
}

int main()
{
    startServer();
    testJson();
    return 0;
}