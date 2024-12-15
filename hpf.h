#pragma once
#include "headers.h"
#include "DataStructures/priQueue.h"

void HPF(int ProcessArrivalQueue)
{
    PCB *CurrentRunningProcess = NULL;
    priQueue *ReadyQueue = CreatePriQueue();
    bool MessagesDone = false;
    int statloc;

    while (ReadyQueue->count != 0 || !MessagesDone)
    {
        // Checking For Terminating Message
        if (msgrcv(ProcessArrivalQueue, NULL, sizeof(msg), 20, IPC_NOWAIT) != -1)
            MessagesDone = true;

        // Retrieving all Process that are sent
        PCB *newProcess;
        msg MsgData;
        while (msgrcv(ProcessArrivalQueue, &MsgData, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->ID = -1;
            newProcess->Priority = MsgData.data.priority;
            newProcess->ArrivalTime = MsgData.data.arrivaltime;
            newProcess->RunningTime = MsgData.data.runningtime;
            newProcess->RemainingTime = MsgData.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->Running = false;
            PriEnqueue(ReadyQueue, &newProcess, newProcess->Priority);
        }

        if (CurrentRunningProcess)
        {
            // handling already running process
            if (waitpid(CurrentRunningProcess->ID, &statloc, WNOHANG) != -1)
            {
                // handling process termination
                CurrentRunningProcess->EndTime = getClk();

                free(CurrentRunningProcess);
            }
            else if (CurrentRunningProcess->Priority < ReadyQueue->front->priority)
            {
                // handling higher priority switch

                kill(CurrentRunningProcess->ID, SIGSTOP);
                CurrentRunningProcess->Running = false;
            }
        }
        else
        {
            // handling no process running

            // handling no process in ready queue
            if (!PriDequeue(ReadyQueue, &CurrentRunningProcess))
                continue;

            // starting a new process (first start)
            if (CurrentRunningProcess->ID == -1)
            {
                CurrentRunningProcess->ID = fork();
                if (CurrentRunningProcess->ID == 0)
                {
                    execl("bin/process.out", "./process.out", NULL);
                }
                else
                {
                    if (CurrentRunningProcess->StartTime == -1)
                        CurrentRunningProcess->StartTime = getClk();
                }
            }
            else
            {
                // handling process that already started but didnt finish
                kill(CurrentRunningProcess->ID, SIGCONT);
                CurrentRunningProcess->Running = true;
            }
        }
    }
    // end of algorithm
    free(ReadyQueue);
}