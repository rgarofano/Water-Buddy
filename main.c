
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void startServer(void)
{
    system("node server/app.js");
}

int main()
{
    startServer();
    return 0;
}