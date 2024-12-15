#include "headers.h"
#include "DataStructures/priQueue.h"
void SJF(int ProcessMessageQueue)
{
    int statloc;
    bool messagedone = false;
    msg sjfmsg;
    priQueue *sjfqueue;
    PCB *runningprocess;
    sjfqueue = CreatePriQueue();
    while (sjfqueue->count!=0||!messagedone){
        if (msgrcv(ProcessMessageQueue,&sjfmsg,sizeof(struct msg),20,IPC_NOWAIT)==-1)
        {
            messagedone =true;
        }
        while (msgrcv(ProcessMessageQueue,&sjfmsg,sizeof(struct msg),1,IPC_NOWAIT)!=-1)
        {
        runningprocess = (PCB*)malloc(sizeof(PCB));
        runningprocess->ID = sjfmsg.data.id;
        runningprocess->Priority = sjfmsg.data.runningtime;
        runningprocess->ArrivalTime = sjfmsg.data.arrivaltime;
        runningprocess->RunningTime=sjfmsg.data.runningtime;
        runningprocess->RemainingTime=sjfmsg.data.runningtime;
        runningprocess->StartTime = -1;
        runningprocess->EndTime = -1;
        runningprocess->WaitTime = 0;
        PriEnqueue(sjfqueue,&runningprocess,runningprocess->Priority);
        pid_t runproc = fork();
        if (runproc == 0)
        {
            execl("bin/process.out","./process.out",NULL);
        }
        else
        {
            //waitpid(runningprocess->ID, &statloc, WNOHANG) != -1
        }

        }
    }

    while (PriDequeue(sjfqueue,&runningprocess))
    {
        pid_t runproc = fork();
        if (runproc == 0)
        {
            execl("bin/process.out","./process.out",NULL);
        }
    }
}
