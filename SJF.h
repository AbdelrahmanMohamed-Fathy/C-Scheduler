#include "headers.h"
#include "DataStructures/priQueue.h"

void SJF(FILE *OutputeFile,int ProcessMessageQueue)
{
    int statloc;
    int CurrentRunningProcessStart;
    bool messagedone = false;
    msg MSGDATA;
    priQueue *SJFqueue;
    PCB *newprocess;
    PCB *currentlyrunningproc = NULL;
    SJFqueue = CreatePriQueue();
    while (SJFqueue->count > 0 || !messagedone || currentlyrunningproc)
    {
        if (msgrcv(ProcessMessageQueue, &MSGDATA, sizeof(struct msg), 20, IPC_NOWAIT) == -1)
        {
            messagedone = true;
        }
        while (msgrcv(ProcessMessageQueue, &MSGDATA, sizeof(struct msg), 1, IPC_NOWAIT) != -1)
        {
            fprintf(OutputeFile, "#process: %d arrived at %d\n", MSGDATA.data.id, MSGDATA.data.arrivaltime);
            newprocess = (PCB *)malloc(sizeof(PCB));
            newprocess->generationID = MSGDATA.data.id;
            newprocess->ID = -1;
            newprocess->Priority = MSGDATA.data.runningtime;
            newprocess->ArrivalTime = MSGDATA.data.arrivaltime;
            newprocess->RunningTime = MSGDATA.data.runningtime;
            newprocess->RemainingTime = MSGDATA.data.runningtime;
            newprocess->StartTime = -1;
            newprocess->EndTime = -1;
            newprocess->WaitTime = 0;
            newprocess->Running = false;
            PriEnqueue(SJFqueue, &newprocess, newprocess->Priority);
        }
        if (currentlyrunningproc)
        {
            PriDequeue(SJFqueue, &currentlyrunningproc);
            pid_t runproc = fork();
            if (runproc == 0)
            {
                char runtime[4];
                sprintf(runtime, "%d", currentlyrunningproc->RunningTime);
                execl("bin/process.out", "./process.out", runtime, NULL);
            }
            else
            {

                if (currentlyrunningproc->StartTime == -1)
                    currentlyrunningproc->StartTime = getClk();
                    CurrentRunningProcessStart = currentlyrunningproc->StartTime;
                    currentlyrunningproc->Running = true;
                    fprintf(OutputeFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", currentlyrunningproc->StartTime, currentlyrunningproc->generationID, currentlyrunningproc->ArrivalTime, currentlyrunningproc->RunningTime, currentlyrunningproc->RemainingTime, currentlyrunningproc->WaitTime);
            }
        }
        else if (waitpid(currentlyrunningproc->ID, &statloc, 0) != -1)
        {
            currentlyrunningproc->Running = false;
            currentlyrunningproc->EndTime = getClk();
            currentlyrunningproc->RemainingTime=0;
            free(currentlyrunningproc);
            currentlyrunningproc = NULL;
        }
    }
}
