#include "headers.h"

void clearResources(int signum);

int ProcessMessageQueue;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    initClk();

    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    printf("scheduler terminating normally.\n");
    destroyClk(false);
}

void clearResources(int signum)
{
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    exit(1);
}