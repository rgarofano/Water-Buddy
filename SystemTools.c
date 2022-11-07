
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SystemTools.h"

// getTimeInMs was copied from Assignment Document
long long getTimeInMs(void) 
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    long long s = spec.tv_sec;
    long long ns = spec.tv_nsec;
    long long totalMs = s * MS_PER_S + ns / NS_PER_MS;

    return totalMs;
}

long long getTimeInUs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    long long s = spec.tv_sec;
    long long ns = spec.tv_nsec;
    long long totalUs = s * US_PER_S + ns / NS_PER_US;

    return totalUs;
}

// sleepForMs was copied from Assignment Document
void sleepForMs(long long sleepTimeMs)
{
    long long sleepTimeNS = sleepTimeMs * NS_PER_MS;
    int s = sleepTimeNS / NS_PER_S;
    int ns = sleepTimeNS % NS_PER_S;
    
    struct timespec sleepTimeSpec = {s, ns};
    nanosleep(&sleepTimeSpec, NULL);
}

void sleepForUs(long long sleepTimeUs)
{
    long long sleepTimeNS = sleepTimeUs * NS_PER_US;
    int s = sleepTimeNS / NS_PER_S;
    int ns = sleepTimeNS % NS_PER_S;
    
    struct timespec sleepTimeSpec = {s, ns};
    nanosleep(&sleepTimeSpec, NULL);
}

FILE* fopenWithCheck(const char* fileName, const char* accessType)
{

    if(fileName == NULL || accessType == NULL) {
        printf("ERROR: NULL arguments in fopenWithCheck()");
        exit(1);
    }

    FILE* pFile = fopen(fileName, accessType);

    if(pFile == NULL) {
        printf("ERROR: Unable to open file %s\n", fileName);
        exit(1);
    }

    return pFile;
}