#pragma once
#include "headers.h"
#include "DataStructures/priQueue.h"

priQueue *ReadyQueue;
PCB *CurrentRunningProcess;
PCB *newProcess;

void HPF(FILE *OutputFile, int ProcessArrivalQueue, cpuData* perfdata)
{
    CurrentRunningProcess = NULL;
    ReadyQueue = CreatePriQueue();
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
            fprintf(OutputFile, "#Recieved Termination message.\n");
        }

        // Retrieving all Process that are sent
        newProcess = NULL;
        while (msgrcv(ProcessArrivalQueue, &MsgData, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            fprintf(OutputFile, "#process: %d arrived at %d\n", MsgData.data.id, MsgData.data.arrivaltime);
            // printf("#process: %d arrived at %d\n",MsgData.data.id,MsgData.data.arrivaltime);
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
            newProcess = NULL;
            // fprintf(OutputFile, "#ReadyQueue has %d processes\n", ReadyQueue->count);
        }

        if (CurrentRunningProcess)
        {
            // handling already running process
            if (waitpid(CurrentRunningProcess->ID, NULL, WNOHANG) == CurrentRunningProcess->ID)
            {
                // handling process termination
                CurrentRunningProcess->EndTime = getClk();
                CurrentRunningProcess->RemainingTime -= (CurrentRunningProcess->EndTime - CurrentRunningProcessStart);
                cpucalculations(perfdata,currentlyrunningproc);
                fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",CurrentRunningProcess->EndTime , CurrentRunningProcess->generationID, CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime,CurrentRunningProcess->WaitTime,CurrentRunningProcess->EndTime - CurrentRunningProcess->ArrivalTime,(CurrentRunningProcess->EndTime - CurrentRunningProcess->ArrivalTime) / (float)(CurrentRunningProcess->RunningTime));
                // printf("At time %d process %d finished arr %d total %d remain %d wait %d\n", CurrentRunningProcess->EndTime, CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                free(CurrentRunningProcess);
                CurrentRunningProcess = NULL;
                continue;
            }
            else if (ReadyQueue->front && CurrentRunningProcess->Priority > ReadyQueue->front->priority)
            {
                // handling higher priority switch
                kill(CurrentRunningProcess->ID, SIGTSTP);
                int now = getClk();
                CurrentRunningProcess->RemainingTime -= (now - CurrentRunningProcessStart);
                CurrentRunningProcess->Running = false;
                fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", now, CurrentRunningProcess->generationID, CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                // printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
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
                    CurrentRunningProcess->WaitTime = CurrentRunningProcess->StartTime - CurrentRunningProcess->ArrivalTime;
                    fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", CurrentRunningProcess->StartTime, CurrentRunningProcess->generationID, CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                    // printf("At time %d process %d started arr %d total %d remain %d wait %d\n",CurrentRunningProcess->StartTime,CurrentRunningProcess->generationID,CurrentRunningProcess->ArrivalTime,CurrentRunningProcess->RunningTime,CurrentRunningProcess->RemainingTime,CurrentRunningProcess->WaitTime);
                    continue;
                }
            }
            else
            {
                // handling process that already started but didnt finish
                CurrentRunningProcessStart = getClk();
                kill(CurrentRunningProcess->ID, SIGCONT);
                CurrentRunningProcess->WaitTime = CurrentRunningProcessStart - CurrentRunningProcess->StartTime - CurrentRunningProcess->RunningTime + CurrentRunningProcess->RemainingTime;
                CurrentRunningProcess->Running = true;
                fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", CurrentRunningProcessStart, CurrentRunningProcess->generationID, CurrentRunningProcess->ArrivalTime, CurrentRunningProcess->RunningTime, CurrentRunningProcess->RemainingTime, CurrentRunningProcess->WaitTime);
                continue;
            }
        }
    }
    // end of algorithm
    // fprintf(OutputFile, "#ReadyQueue has %d processes\n", ReadyQueue->count);
    free(ReadyQueue);
}

void HPFFree()
{
    PCB* Dummy;
    if (ReadyQueue)
    {
        while (PriDequeue(ReadyQueue,&Dummy))
            free(Dummy);
        free(ReadyQueue);
    }
    if (CurrentRunningProcess)
        free(CurrentRunningProcess);
    if (newProcess)
        free(newProcess);
    return;
}