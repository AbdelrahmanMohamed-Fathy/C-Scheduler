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

    while (ReadyQueue->count > 0 || !MessagesDone || CurrentRunningProcess)
    {
        msg MsgData;
        // Checking For Terminating Message
        if (msgrcv(ProcessArrivalQueue, &MsgData, sizeof(msg), 20, IPC_NOWAIT) != -1)
        {
            MessagesDone = true;
            fprintf(OutputeFile,"#Recieved Termination message.\n");
        }
            

        // Retrieving all Process that are sent
        PCB *newProcess = NULL;
        while (msgrcv(ProcessArrivalQueue, &MsgData, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            fprintf(OutputeFile,"#process: %d arrived at %d\n",MsgData.data.id,MsgData.data.arrivaltime);
            //printf("#process: %d arrived at %d\n",MsgData.data.id,MsgData.data.arrivaltime);
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
            if (waitpid(CurrentRunningProcess->ID, NULL, WNOHANG) == CurrentRunningProcess->ID)
            {
                // handling process termination
                CurrentRunningProcess->EndTime = getClk();
                CurrentRunningProcess->RemainingTime = 0;
                //
                fprintf(OutputeFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", CurrentRunningProcess->EndTime, CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                //printf("At time %d process %d finished arr %d total %d remain %d wait %d\n", CurrentRunningProcess->EndTime, CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                free(CurrentRunningProcess);
                CurrentRunningProcess = NULL;
                continue;
            }
            else if (ReadyQueue->front && CurrentRunningProcess->Priority > ReadyQueue->front->priority)
            {
                // handling higher priority switch
                int now = getClk();
                kill(CurrentRunningProcess->ID, SIGSTOP);
                CurrentRunningProcess->RemainingTime -= (now-CurrentRunningProcessStart);
                CurrentRunningProcess->Running = false;
                fprintf(OutputeFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", now, CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                //printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
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
                    //printf("At time %d process %d started arr %d total %d remain %d wait %d\n",CurrentRunningProcess->StartTime,CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime,CurrentRunningProcess->RunningTime,CurrentRunningProcess->RemainingTime,CurrentRunningProcess->WaitTime);
                    continue;
                }
            }
            else
            {
                // handling process that already started but didnt finish
                CurrentRunningProcessStart = getClk();
                kill(CurrentRunningProcess->ID, SIGCONT);
                CurrentRunningProcess->Running = true;
                fprintf(OutputeFile,"At time %d process %d resumed arr %d total %d remain %d wait %d\n",CurrentRunningProcessStart,CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime,CurrentRunningProcess->RunningTime,CurrentRunningProcess->RemainingTime,CurrentRunningProcess->WaitTime);
                continue;
            }
        }
    }
    // end of algorithm
    free(ReadyQueue);
}