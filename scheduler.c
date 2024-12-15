#include "headers.h"
#include "DataStructures/priQueue.h"
#include "pcb.h"

void clearResources(int signum);

int ProcessMessageQueue;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    initClk();

    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    SJF();
    // TODO: implement the scheduler.
    // TODO: upon termination release the clock resources.

    printf("scheduler terminating normally.\n");
    destroyClk(false);
    clearResources(0);
}

void clearResources(int signum)
{
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    exit(1);
}

void SJF()
{
    msg sjfmsg;
    priQueue *sjfqueue;
    PCB *runningprocess;
    sjfqueue = CreatePriQueue();
    while (msgrcv(ProcessMessageQueue,&sjfmsg,sizeof(struct msg),0,IPC_NOWAIT)!=-1)
    {
        runningprocess = (PCB*)malloc(sizeof(PCB));
        runningprocess->ID = sjfmsg.data.id;
        runningprocess->Priority = sjfmsg.data.runningtime;
        runningprocess->ArrivalTime = sjfmsg.data.arrivaltime;
        runningprocess->RunningTime=sjfmsg.data.runningtime;
        runningprocess->RemainingTime=sjfmsg.data.runningtime;
        runningprocess->StartTime= -1;
        runningprocess->EndTime = -1;
        runningprocess->WaitTime = 0;
        PriEnqueue(sjfqueue,&runningprocess,runningprocess->Priority);
        //schmsg.data.processinfo.RemainingTime = schmsg.data.runningtime;
    }

    while (PriDequeue(sjfqueue,&runningprocces))
    {
        while (runningprocces->processinfo.RemainingTime > 0)
        {
            int time;
            printf("process numer with id %d and runtime %d is running\n ",runningprocces->id,runningprocces->runningtime);
            time = getClk();
            printf("remaining time = %d      clock =%d ",runningprocces->processinfo.RemainingTime,time);
            runningprocces->processinfo.RemainingTime-- ;
        }
        printf("sjf done");
    }
}