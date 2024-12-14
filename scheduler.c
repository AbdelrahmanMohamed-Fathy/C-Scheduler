#include "headers.h"

void clearResources(int signum);

int ProcessMessageQueue;

int main(int argc, char *argv[])
{
    initClk();

    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    destroyClk(false);
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    printf("scheduler terminating normally.\n");
}

void clearResources(int signum)
{
    printf("scheduler terminating abnormally.\n");
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    exit(1);
}