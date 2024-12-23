#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

#define NUM_QUEUES 11 // Number of priority levels
priQueue *queues[NUM_QUEUES];
PCB *runningProcess;
PCB *newProcess;

void MLFQ(FILE *OutputFile, FILE *MemFile, int ProcessMessageQueue, int quantum, cpuData *perfdata, MemTree *MemoryTree)
{
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        queues[i] = CreatePriQueue();
    }

    int time;
    int runningProcessStart;
    int CurrentQueue = 0;
    int StartQueue = 0;
    int GivenQuantum;
    bool messagesdone = false;
    bool queuesEmpty = true;
    runningProcess = NULL;
    msg MLFQmsg;

    while (!queuesEmpty || !messagesdone || runningProcess)
    {
        queuesEmpty = true;
        for (int i = 0; i < NUM_QUEUES; i++)
        {
            if (queues[i]->front)
            {
                queuesEmpty = false;
                break;
            }
        }
        if (msgrcv(ProcessMessageQueue, &MLFQmsg, sizeof(msg), 20, IPC_NOWAIT) != -1)
        {
            messagesdone = true;
            fprintf(OutputFile, "#Recieved Termination message.\n");
        }
        // receive new processes and add to the highest priority queue (Level 0)
        newProcess = NULL;
        while (msgrcv(ProcessMessageQueue, &MLFQmsg, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = MLFQmsg.data.id;
            newProcess->ID = -1;
            newProcess->Priority = MLFQmsg.data.priority;
            newProcess->originalPriority = MLFQmsg.data.priority;
            newProcess->ArrivalTime = MLFQmsg.data.arrivaltime;
            newProcess->RunningTime = MLFQmsg.data.runningtime;
            newProcess->RemainingTime = MLFQmsg.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->lastend = 0;
            newProcess->Running = false;
            newProcess->Size = MLFQmsg.data.memsize;
            PriEnqueue(queues[newProcess->Priority], &newProcess, newProcess->ArrivalTime);
            fprintf(OutputFile, "#process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, newProcess->ArrivalTime, newProcess->RunningTime);
            newProcess = NULL;
            queuesEmpty = false;
            // printf("#process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, newProcess->ArrivalTime, newProcess->RunningTime);
        }

        if (runningProcess)
        {
            // handling already running process
            if (runningProcessStart + GivenQuantum - getClk() > 0)
                continue;

            else
            {
                if (runningProcess->RemainingTime <= GivenQuantum)
                {
                    printf("Attempting to end process %d, remaining time is %d\n", runningProcess->ID, runningProcess->RemainingTime);
                    if (waitpid(runningProcess->ID, NULL, 0) == runningProcess->ID)
                    {
                        runningProcess->EndTime = getClk();
                        printf("process %d, Ended at %d\n", runningProcess->ID, runningProcess->EndTime);
                        runningProcess->RemainingTime -= GivenQuantum;
                        runningProcess->Running = false;
                        cpucalculations(perfdata, runningProcess);
                        TreeFree(MemoryTree, runningProcess->Location.Start);
                        fprintf(MemFile, "At time %d freed %d bytes for process %d from %d to %d\n", runningProcess->EndTime, runningProcess->Size, runningProcess->generationID, runningProcess->Location.Start, runningProcess->Location.End);
                        fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n", runningProcess->EndTime, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime, runningProcess->EndTime - runningProcess->ArrivalTime, (runningProcess->EndTime - runningProcess->ArrivalTime) / (float)(runningProcess->RunningTime));
                        free(runningProcess);
                        runningProcess = NULL;
                        continue;
                    }
                }
                else
                {
                    int now = getClk();
                    kill(runningProcess->ID, SIGSTOP);
                    PriEnqueue(queues[(runningProcess->Priority + 1 > 10) ? (10) : (runningProcess->Priority + 1)], &runningProcess, now);
                    queuesEmpty = false;
                    runningProcess->Priority = (runningProcess->Priority + 1 > 10) ? (10) : (runningProcess->Priority + 1);
                    runningProcess->Running = false;
                    runningProcess->RemainingTime -= GivenQuantum;
                    runningProcess->lastend = now;
                    fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", now, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                    runningProcess = NULL;
                    continue;
                }
            }
        }
        else
        {
            // handling no running process
            bool topQueuesEmpty = true;
            for (int i = 0; i < NUM_QUEUES - 1; i++) // Handling returning processes to their original levels
            {
                if (!isPriQueueEmpty(queues[i]))
                {
                    topQueuesEmpty = false;
                    break;
                }
            }
            if (topQueuesEmpty && !isPriQueueEmpty(queues[NUM_QUEUES - 1]))
            {
                PCB *targetProcess = NULL;
                for (int i = 0; i < queues[NUM_QUEUES - 1]->count; i++)
                {
                    PriDequeue(queues[NUM_QUEUES - 1], &targetProcess);
                    if (targetProcess->originalPriority != targetProcess->Priority)
                    {
                        fprintf(OutputFile, "#Process with ID %d was restored to it's original level %d, was at %d\n", targetProcess->generationID, targetProcess->originalPriority, targetProcess->Priority);
                        printf("#At time %d, Process with ID %d was restored to it's original level %d, was at %d, remaining time %d\n", getClk(), targetProcess->generationID, targetProcess->originalPriority, targetProcess->Priority, targetProcess->RemainingTime);
                        targetProcess->Priority = targetProcess->originalPriority;
                    }
                    PriEnqueue(queues[targetProcess->Priority], &targetProcess, getClk());
                }
            }
            bool Found = false;
            for (int i = 0; i < NUM_QUEUES; i++)
            {
                if (PriDequeue(queues[i], &runningProcess))
                {
                    Found = true;
                    break;
                }
            }
            if (!Found)
            {
                StartQueue = (StartQueue + 1) % NUM_QUEUES;
                CurrentQueue = StartQueue;
                continue;
            }
            else
            {
                if (runningProcess->ID == -1)
                {
                    // handling process that never started before
                    runningProcess->ID = fork();
                    if (runningProcess->ID == 0)
                    {
                        // Child
                        char runtime[4];
                        sprintf(runtime, "%d", runningProcess->RunningTime);
                        execl("bin/process.out", "./process.out", runtime, NULL);
                    }
                    else
                    {
                        // setting up PCB for first start
                        runningProcess->StartTime = getClk();
                        runningProcessStart = runningProcess->StartTime;
                        runningProcess->Running = true;
                        runningProcess->WaitTime = runningProcess->StartTime - runningProcess->ArrivalTime;
                        GivenQuantum = (runningProcess->RemainingTime <= quantum) ? (runningProcess->RemainingTime) : (quantum);
                        MemLocation *allocatedLocation = TreeAllocate(MemoryTree, runningProcess->Size);
                        runningProcess->Location = *allocatedLocation;
                        fprintf(MemFile, "At time %d allocated %d bytes for process %d from %d to %d\n", runningProcess->StartTime, runningProcess->Size, runningProcess->generationID, runningProcess->Location.Start, runningProcess->Location.End);
                        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", runningProcess->StartTime, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                    }
                }
                else
                {
                    // process already started but stopped
                    runningProcessStart = getClk();
                    kill(runningProcess->ID, SIGCONT);
                    runningProcess->Running = true;
                    runningProcess->WaitTime += runningProcessStart - runningProcess->lastend;
                    GivenQuantum = (runningProcess->RemainingTime <= quantum) ? (runningProcess->RemainingTime) : (quantum);
                    fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", runningProcessStart, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                }
            }
        }
    }
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        free(queues[i]);
    }
}

void MLFQFree()
{
    PCB *Dummy;
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        if (queues[i])
        {
            while (PriDequeue(queues[i], &Dummy))
                free(Dummy);
            free(queues[i]);
        }
    }
    if (runningProcess)
        free(runningProcess);
    if (newProcess)
        free(newProcess);
    return;
}