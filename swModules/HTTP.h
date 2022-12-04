/* Used to send HTTP get and post request to a node.js 
   server running on localhost                         */

#ifndef HTTP_H
#define HTTP_H

void HTTP_sendGetRequest(char* route);
void HTTP_sendPostRequest(char* route);

#endif