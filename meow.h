#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

#define NUM_QUEUES 3  // Number of priority levels
int queue_quantum[NUM_QUEUES] = {2, 4, 8}; // Quantum for each level

void MLFQ(FILE *OutputFile, int ProcessMessageQueue)
{
    CircQueue *queues[NUM_QUEUES];
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        queues[i] = CreatecircQueue();
    }

    int time;
    bool messagesdone = false;
    msg MLFQmsg;
    PCB *runningprocess = NULL;
    PCB *newProcess;

    while (!isCircQueueEmpty(RRqueue) || !messagesdone)
    {
        //receive new processes and add to the highest priority queue (Level 0)
        while (msgrcv(ProcessMessageQueue, &MLFQmsg, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = MLFQmsg.data.id;
            newProcess->ID = -1; 
            newProcess->Priority = 0; //start in highest priority queue
            newProcess->ArrivalTime = MLFQmsg.data.arrivaltime;
            newProcess->RunningTime = MLFQmsg.data.runningtime;
            newProcess->RemainingTime = MLFQmsg.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->Running = false;
            CircEnqueue(queues[0], &newProcess);
            fprintf(OutputFile, "process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, newProcess->ArrivalTime, newProcess->RunningTime);
        }

        //find the highest-priority non-empty queue
        int currentQueue = -1;
        for (int i = 0; i < NUM_QUEUES; i++)
        {
            if (!isCircQueueEmpty(queues[i]))
            {
                currentQueue = i;
                break;
            }
        }

        if (currentQueue == -1) //no processes left
        {
            if (messagesdone) break; //exit if no new messages
            continue;
        }

        //dequeue process from the current queue
        CircDequeue(queues[currentQueue], &runningprocess);

        if (runningprocess->StartTime == -1)
        {
            //first time running the process
            runningprocess->ID = fork();
            if (runningprocess->ID == 0)
            {
                char runtime[4];
                sprintf(runtime, "%d", runningprocess->RunningTime);
                execl("bin/process.out", "./process.out", runtime, NULL);
                perror("execl failed");
                exit(1);
            }
            else if (runningprocess->ID < 0)
            {
                perror("fork failed");
                exit(1);
            }
            runningprocess->StartTime = getClk();
        }

        //running the processes algorithm same as rr
        int quantum = queue_quantum[currentQueue];
        if (runningprocess->RemainingTime <= quantum)
        {
            //process finishes within its quantum
            kill(runningprocess->ID, SIGCONT);
            time = getClk();
            while (getClk() < (time + runningprocess->RemainingTime));
            runningprocess->RemainingTime = 0;
            runningprocess->EndTime = getClk();
            fprintf(OutputFile, "process with id=%d and runningtime=%d finished at %d \n", runningprocess->generationID, runningprocess->RunningTime, runningprocess->EndTime);
            kill(runningprocess->ID, SIGKILL);
            free(runningprocess);
        }
        else
        {
            //process exceeds its quantum
            kill(runningprocess->ID, SIGCONT);
            time = getClk();
            while (getClk() < (time + quantum));
            runningprocess->RemainingTime -= quantum;
            kill(runningprocess->ID, SIGSTOP);
            fprintf(OutputFile, "process with id=%d remaining time=%d clock=%d \n", runningprocess->generationID, runningprocess->RemainingTime, getClk());

            //demote the process to a lower-priority queue
            int nextQueue = currentQueue + 1;
            if (nextQueue < NUM_QUEUES)
            {
                CircEnqueue(queues[nextQueue], &runningprocess);
            }
            else
            {
                //lowest-priority queue (re-enqueue in the same queue)
                CircEnqueue(queues[currentQueue], &runningprocess);
            }
        }
    }

    for (int i = 0; i < NUM_QUEUES; i++)
    {
        free(queues[i]);
    }
    printf("MLFQ done\n");
}
