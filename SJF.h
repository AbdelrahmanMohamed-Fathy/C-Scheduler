#include "headers.h"
#include "DataStructures/priQueue.h"

void SJF(int ProcessMessageQueue)
{
    int statloc;
    bool messagedone = false;
    msg sjfmsg;
    priQueue *sjfqueue;
    PCB *sjfproc;
    PCB *currentlyrunningproc = NULL;
    sjfqueue = CreatePriQueue();
    while (sjfqueue->count != 0 || !messagedone || !currentlyrunningproc)
    {
        if (msgrcv(ProcessMessageQueue, &sjfmsg, sizeof(struct msg), 20, IPC_NOWAIT) == -1)
        {
            messagedone = true;
        }
        while (msgrcv(ProcessMessageQueue, &sjfmsg, sizeof(struct msg), 1, IPC_NOWAIT) != -1)
        {
            sjfproc = (PCB *)malloc(sizeof(PCB));
            sjfproc->generationID = sjfmsg.data.id;
            sjfproc->ID = -1;
            sjfproc->Priority = sjfmsg.data.runningtime;
            sjfproc->ArrivalTime = sjfmsg.data.arrivaltime;
            sjfproc->RunningTime = sjfmsg.data.runningtime;
            sjfproc->RemainingTime = sjfmsg.data.runningtime;
            sjfproc->StartTime = -1;
            sjfproc->EndTime = -1;
            sjfproc->WaitTime = 0;
            sjfproc->Running = false;
            PriEnqueue(sjfqueue, &sjfproc, sjfproc->Priority);
        }
        if (!currentlyrunningproc)
        {
            PriDequeue(sjfqueue, &currentlyrunningproc);
            pid_t runproc = fork();
            if (runproc == 0)
            {
                execl("bin/process.out", "./process.out", NULL);
            }
            else
            {
                currentlyrunningproc->StartTime = getClk();
                currentlyrunningproc->Running = true;
            }
        }
        else if (waitpid(currentlyrunningproc->ID, &statloc, WNOHANG) != -1)
        {
            currentlyrunningproc->Running = false;
            currentlyrunningproc->EndTime = getClk();
            ///////////////////here nigga
            free(currentlyrunningproc);
            currentlyrunningproc = NULL;
        }
    }
}
