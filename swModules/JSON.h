/* Parses JSON file and places the content into a struct. Must be 
   compiled with the ljson-c flag                                 */

#ifndef JSON_H
#define JSON_H

#include <stdbool.h>

#include "User.h"

// Returns whether it was successful or not (true on successful, false on fail)
bool JSON_getUserDataFromFile(char* filePath, user_t* userData);

#endif