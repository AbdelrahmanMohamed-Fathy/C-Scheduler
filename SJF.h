#include "headers.h"
#include "DataStructures/priQueue.h"

priQueue *SJFqueue;
PCB *newprocess;
PCB *currentlyrunningproc;

void SJF(FILE *OutputFile, FILE *MemFile, int ProcessMessageQueue, cpuData *perfdata, MemTree *MemoryTree)
{
    int statloc;
    bool messagedone = false;
    msg MSGDATA;
    currentlyrunningproc = NULL;
    SJFqueue = CreatePriQueue();
    while (SJFqueue->count > 0 || !messagedone || currentlyrunningproc)
    {
        if (msgrcv(ProcessMessageQueue, &MSGDATA, sizeof(struct msg), 20, IPC_NOWAIT) != -1)
        {
            messagedone = true;
        }
        newprocess = NULL;
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
            newprocess->Size = MSGDATA.data.memsize;
            PriEnqueue(SJFqueue, &newprocess, newprocess->Priority);
            newprocess = NULL;
        }
        if (PriDequeue(SJFqueue, &currentlyrunningproc))
        {
            currentlyrunningproc->ID = fork();
            if (currentlyrunningproc->ID == 0)
            {
                char runtime[4];
                sprintf(runtime, "%d", currentlyrunningproc->RunningTime);
                execl("bin/process.out", "./process.out", runtime, NULL);
            }
            else
            {
                currentlyrunningproc->StartTime = getClk();
                currentlyrunningproc->WaitTime = currentlyrunningproc->StartTime - currentlyrunningproc->ArrivalTime;
                MemLocation *allocatedLocation = TreeAllocate(MemoryTree, currentlyrunningproc->Size);
                currentlyrunningproc->Location = *allocatedLocation;
                fprintf(MemFile, "At time %d allocated %d bytes for process %d from %d to %d\n", currentlyrunningproc->StartTime, currentlyrunningproc->Size, currentlyrunningproc->generationID, currentlyrunningproc->Location.Start, currentlyrunningproc->Location.End);
                fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", currentlyrunningproc->StartTime, currentlyrunningproc->generationID, currentlyrunningproc->ArrivalTime, currentlyrunningproc->RunningTime, currentlyrunningproc->RemainingTime, currentlyrunningproc->WaitTime);
                if (waitpid(currentlyrunningproc->ID, NULL, 0) != -1)
                {
                    currentlyrunningproc->EndTime = getClk();
                    currentlyrunningproc->RemainingTime = 0;
                    cpucalculations(perfdata, currentlyrunningproc);
                    TreeFree(MemoryTree, currentlyrunningproc->Location.Start);
                    fprintf(MemFile, "At time %d freed %d bytes for process %d from %d to %d\n", currentlyrunningproc->EndTime, currentlyrunningproc->Size, currentlyrunningproc->generationID, currentlyrunningproc->Location.Start, currentlyrunningproc->Location.End);
                    fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n", currentlyrunningproc->EndTime, currentlyrunningproc->generationID, currentlyrunningproc->ArrivalTime, currentlyrunningproc->RunningTime, currentlyrunningproc->RemainingTime, currentlyrunningproc->WaitTime, currentlyrunningproc->EndTime - currentlyrunningproc->ArrivalTime, (currentlyrunningproc->EndTime - currentlyrunningproc->ArrivalTime) / (float)(currentlyrunningproc->RunningTime));
                    free(currentlyrunningproc);
                    currentlyrunningproc = NULL;
                    continue;
                }
            }
        }
    }
}

void SJFFree()
{
    PCB *Dummy;
    if (SJFqueue)
    {
        while (PriDequeue(SJFqueue, &Dummy))
            free(Dummy);
        free(SJFqueue);
    }
    if (currentlyrunningproc)
        free(currentlyrunningproc);
    if (newprocess)
        free(newprocess);
    return;
}