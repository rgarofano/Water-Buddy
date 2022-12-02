#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3000
#define SERVER_IP "192.168.7.2"
#define GET_REQUEST_STR "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n"
#define POST_REQUEST_STR "POST %s HTTP/1.1\r\nHost: %s\r\n\r\n"

#define BUFF_SIZE 1024

static void sendHttpRequest(int socketFileDescriptor, char* request)
{
    ssize_t bytesSent = 0;
    int requestLength = strlen(request);
    while (bytesSent < requestLength) {
        ssize_t bytes = write(socketFileDescriptor, request + bytesSent, requestLength - bytesSent);
        
        if (bytes == -1) {
            perror("unable to write");
            exit(EXIT_FAILURE);
        }
        
        bytesSent += bytes;
    }

    char buffer[BUFF_SIZE];
    while ((bytesSent = read(socketFileDescriptor, buffer, BUFSIZ)) > 0) {
        write(STDOUT_FILENO, buffer, bytesSent);
    }
    
    if (bytesSent == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // required for formatiing
    printf("\n");
}

static void buildDestinationAddress(struct sockaddr_in* sockaddr_in)
{
    struct hostent* hostent = gethostbyname(SERVER_IP);
    if (hostent == NULL) {
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", SERVER_IP);
    }

    in_addr_t in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
    }

    sockaddr_in->sin_addr.s_addr = in_addr;
    sockaddr_in->sin_family = AF_INET;
    sockaddr_in->sin_port = htons(PORT);
}

static void sendRequestToServer(char* request)
{
    // Create socket to send data over the network
    struct protoent * protoent = getprotobyname("tcp");
    if (protoent == NULL) {
        perror("getprotobyname");
    }

    int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    if (socketFileDescriptor == -1) {
        perror("socket");
    }
    
    struct sockaddr_in sockaddr_in;
    buildDestinationAddress(&sockaddr_in);

    // Connect to server and then send the request
    if (connect(socketFileDescriptor, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
        perror("unable to connect");
    }

    sendHttpRequest(socketFileDescriptor, request);

    // close the connection
    close(socketFileDescriptor);
}

void HTTP_sendGetRequest(char* route)
{
    char request[BUFF_SIZE];
    int requestLength = snprintf(request, BUFF_SIZE, GET_REQUEST_STR, route, SERVER_IP);
    if (requestLength >= BUFF_SIZE) {
        fprintf(stderr, "request length large: %d\n", requestLength);
        exit(EXIT_FAILURE);
    }
    sendRequestToServer(request);
}

void HTTP_sendPostRequest(char* route)
{
    char buffer[BUFF_SIZE];
    snprintf(buffer, BUFF_SIZE, POST_REQUEST_STR, route);
    sendRequestToServer(buffer);
}