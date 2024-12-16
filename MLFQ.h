#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

#define NUM_QUEUES 11 // Number of priority levels

void MLFQ(FILE *OutputFile, int ProcessMessageQueue, int Quantum)
{
    CircQueue *queues[NUM_QUEUES];
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        queues[i] = CreatecircQueue();
    }

    int time;
    bool MessagesDone = false;
    msg MsgData;
    PCB *CurrentRunningProcess = NULL;
    PCB *newProcess;

    while (/* check for if queues still have processes inside ||*/ !MessagesDone || CurrentRunningProcess)
    {
        // receive new processes and add to the highest priority queue (Level 0)
        while (msgrcv(ProcessMessageQueue, &MsgData, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            fprintf(OutputFile, "#process: %d arrived at %d\n", MsgData.data.id, MsgData.data.arrivaltime);
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
            CircEnqueue(queues[newProcess->Priority - 1], &newProcess);
        }

        // find the highest-priority non-empty queue
        int currentQueue = -1;
        for (int i = 0; i < NUM_QUEUES; i++)
        {
            if (!isCircQueueEmpty(queues[i]))
            {
                currentQueue = i;
                break;
            }
        }

        if (currentQueue == -1) // no processes left
            continue;

        // dequeue process from the current queue
        CircDequeue(queues[currentQueue], &CurrentRunningProcess);

        if (CurrentRunningProcess->StartTime == -1)
        {
            // first time running the process
            CurrentRunningProcess->ID = fork();
            if (CurrentRunningProcess->ID == 0)
            {
                char runtime[4];
                sprintf(runtime, "%d", CurrentRunningProcess->RunningTime);
                execl("bin/process.out", "./process.out", runtime, NULL);
                perror("execl failed");
                exit(1);
            }
            else if (CurrentRunningProcess->ID < 0)
            {
                perror("fork failed");
                exit(1);
            }
            CurrentRunningProcess->StartTime = getClk();
        }

        // running the processes algorithm same as rr
        if (CurrentRunningProcess->RemainingTime <= Quantum)
        {
            // process finishes within its quantum
            kill(CurrentRunningProcess->ID, SIGCONT);
            time = getClk();
            while (getClk() < (time + CurrentRunningProcess->RemainingTime))
                continue;
            CurrentRunningProcess->RemainingTime = 0;
            CurrentRunningProcess->EndTime = getClk();
            fprintf(OutputFile, "process with id=%d and runningtime=%d finished at %d \n", CurrentRunningProcess->generationID, CurrentRunningProcess->RunningTime, CurrentRunningProcess->EndTime);
            kill(CurrentRunningProcess->ID, SIGKILL);
            free(CurrentRunningProcess);
        }
        else
        {
            // process exceeds its quantum
            kill(CurrentRunningProcess->ID, SIGCONT);
            time = getClk();
            while (getClk() < (time + Quantum))
                continue;
            CurrentRunningProcess->RemainingTime -= Quantum;
            kill(CurrentRunningProcess->ID, SIGSTOP);
            fprintf(OutputFile, "process with id=%d remaining time=%d clock=%d \n", CurrentRunningProcess->generationID, CurrentRunningProcess->RemainingTime, getClk());

            // demote the process to a lower-priority queue
            int nextQueue = currentQueue + 1;
            if (nextQueue < NUM_QUEUES)
            {
                CircEnqueue(queues[nextQueue], &CurrentRunningProcess);
            }
            else
            {
                // lowest-priority queue (re-enqueue in the same queue)
                CircEnqueue(queues[currentQueue], &CurrentRunningProcess);
            }
        }
    }

    for (int i = 0; i < NUM_QUEUES; i++)
    {
        free(queues[i]);
    }
    printf("MLFQ done\n");
}
