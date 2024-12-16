#include "headers.h"
#include "DataStructures/priQueue.h"

void SJF(FILE *OutputFile,int ProcessMessageQueue)
{
    int statloc;
    bool messagedone = false;
    msg MSGDATA;
    priQueue *SJFqueue;
    PCB *newprocess;
    PCB *currentlyrunningproc = NULL;
    SJFqueue = CreatePriQueue();
    while (SJFqueue->count > 0 || !messagedone || currentlyrunningproc)
    {
        if (msgrcv(ProcessMessageQueue, &MSGDATA, sizeof(struct msg), 20, IPC_NOWAIT) != -1)
        {
            messagedone = true;
        }
        while (msgrcv(ProcessMessageQueue, &MSGDATA, sizeof(struct msg), 1, IPC_NOWAIT) != -1)
        {
            fprintf(OutputFile, "#process: %d arrived at %d\n", MSGDATA.data.id, MSGDATA.data.arrivaltime);
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
        if(PriDequeue(SJFqueue,&currentlyrunningproc))
        {
            currentlyrunningproc->ID = fork();
            if(currentlyrunningproc->ID==0)
            {
                char runtime[4];
                sprintf(runtime, "%d", currentlyrunningproc->RunningTime);
                execl("bin/process.out", "./process.out", runtime, NULL);
            }
            else
            {
                currentlyrunningproc->StartTime=getClk();
                fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", currentlyrunningproc->StartTime, currentlyrunningproc->generationID, currentlyrunningproc->ArrivalTime, currentlyrunningproc->RunningTime, currentlyrunningproc->RemainingTime, currentlyrunningproc->WaitTime);
                if(waitpid(currentlyrunningproc->ID, NULL, 0) != -1)
                {
                    currentlyrunningproc->EndTime=getClk();
                    currentlyrunningproc->RemainingTime=0;
                    fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", currentlyrunningproc->EndTime, currentlyrunningproc->generationID, currentlyrunningproc->ArrivalTime, currentlyrunningproc->RunningTime, currentlyrunningproc->RemainingTime, currentlyrunningproc->WaitTime);
                    free(currentlyrunningproc);
                    currentlyrunningproc=NULL;
                    continue;
                }
            }
        }
    }
}