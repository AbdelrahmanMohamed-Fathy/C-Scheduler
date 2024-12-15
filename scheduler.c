#include "headers.h"
#include "DataStructures/priQueue.h"


int schmsgid;
msg schmsg;
schmsgid = msgget(MSGKEY, IPC_CREAT | 0666);
if (schmsgid==-1)
{
    printf("Error in creating ready queue \n");
    exit(-1);
}

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
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    destroyClk(false);
}

void clearResources(int signum)
{
    printf("scheduler terminating abnormally.\n");
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    exit(1);
}

void SJF()
{
    priQueue *sjfqueue;
    sjfqueue = CreatePriQueue();
    while (msgrcv(schmsgid,&schmsg,sizeof(schmsg),0,IPC_NOWAIT)!=-1)
    {
        
    }


}