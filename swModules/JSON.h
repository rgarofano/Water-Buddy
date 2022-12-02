/* Parses JSON file and places the content into a struct. Must be 
   compiled with the ljson-c flag                                 */

#ifndef JSON_H
#define JSON_H

#include "User.h"

void JSON_getUserDataFromFile(char* filePath, user_t* userData);

#endif