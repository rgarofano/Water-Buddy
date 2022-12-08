/* Parses JSON file which has data inputted form the user 
   and places the content into a user struct              */

#ifndef JSON_H
#define JSON_H

#include <stdbool.h>

#include "User.h"

// Returns whether it was successful or not (true on successful, false on fail)
bool JSON_getUserDataFromFile(char* filePath, user_t* userData);

#endif
