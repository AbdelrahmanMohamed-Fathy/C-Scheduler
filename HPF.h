#pragma once
#include "headers.h"
#include "DataStructures/priQueue.h"

void HPF(FILE* OutputeFile,int ProcessArrivalQueue)
{
    PCB *CurrentRunningProcess = NULL;
    priQueue *ReadyQueue = CreatePriQueue();
    bool MessagesDone = false;
    int CurrentRunningProcessStart;
    int statloc;

    while (ReadyQueue->count != 0 || !MessagesDone || !CurrentRunningProcess)
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
            newProcess->generationID = MsgData.data.id;
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
            if (waitpid(CurrentRunningProcess->ID, &statloc, WNOHANG) < 0)
            {
                // handling process termination
                CurrentRunningProcess->EndTime = getClk();
                CurrentRunningProcess->RemainingTime = 0;
                //
                fprintf(OutputeFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", getClk(), CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                free(CurrentRunningProcess);
                CurrentRunningProcess = NULL;
                continue;
            }
            else if (CurrentRunningProcess->Priority > ReadyQueue->front->priority)
            {
                // handling higher priority switch
                kill(CurrentRunningProcess->ID, SIGSTOP);
                CurrentRunningProcess->RemainingTime -= (getClk()-CurrentRunningProcessStart);
                CurrentRunningProcess->Running = false;
                PriEnqueue(ReadyQueue, &CurrentRunningProcess, CurrentRunningProcess->Priority);
                CurrentRunningProcess = NULL;
                continue;
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
                    char runtime[4];
                    sprintf(runtime, "%d", CurrentRunningProcess->RunningTime);
                    execl("bin/process.out", "./process.out", runtime, NULL);
                }
                else
                {
                    if (CurrentRunningProcess->StartTime == -1)
                        CurrentRunningProcess->StartTime = getClk();
                    CurrentRunningProcessStart = CurrentRunningProcess->StartTime;
                    CurrentRunningProcess->Running = true;
                    fprintf(OutputeFile,"At time %d process %d started arr %d total %d remain %d wait %d\n",CurrentRunningProcess->StartTime,CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime,CurrentRunningProcess->RunningTime,CurrentRunningProcess->RemainingTime,CurrentRunningProcess->WaitTime);
                }
            }
            else
            {
                // handling process that already started but didnt finish
                kill(CurrentRunningProcess->ID, SIGCONT);
                CurrentRunningProcessStart = getClk();
                CurrentRunningProcess->Running = true;
            }
        }
    }
    // end of algorithm
    free(ReadyQueue);
}