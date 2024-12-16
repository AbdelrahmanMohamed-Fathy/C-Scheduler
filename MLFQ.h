#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

#define NUM_QUEUES 11                       // Number of priority levels


void MLFQ(FILE *OutputFile, int ProcessMessageQueue, int quantum)
{
    CircQueue *queues[NUM_QUEUES];
    for (int i = 0; i < NUM_QUEUES; i++)
    {
        queues[i] = CreatecircQueue();
    }

    int time;
    int runningProcessStart;
    bool messagesdone = false;
    msg MLFQmsg;
    PCB *runningProcess = NULL;
    PCB *newProcess;

    while (!isCircQueueEmpty(*queues) || !messagesdone)
    {
        // receive new processes and add to the highest priority queue (Level 0)
        while (msgrcv(ProcessMessageQueue, &MLFQmsg, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = MLFQmsg.data.id;
            newProcess->ID = -1;
            newProcess->Priority = 0; // start in highest priority queue
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

        if (!runningProcess) //I'm not working on a process at the moment, have to dequeue a new one
        {
            for (int i = 0; i < 11; i++)
            {
                if (CircDequeue(queues[i], &runningProcess))
                {
                    runningProcess->Priority = i;
                    break;
                }
            }

            if (!runningProcess)
                continue;

            if (runningProcess->ID == -1) //A non forked process, so we start anew
            {
                runningProcess->ID = fork();
                if (runningProcess->ID == 0)
                {
                    char runtime[4];
                    sprintf(runtime, "%d", runningProcess->RunningTime);
                    execl("bin/process.out", "./process.out", runtime, NULL);
                }
                else
                {
                    runningProcess->StartTime = getClk();
                    runningProcessStart = runningProcess->StartTime;
                    runningProcess->Running = true;
                    runningProcess->WaitTime = runningProcess->StartTime - runningProcess->ArrivalTime;
                    fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", runningProcess->StartTime, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                }
            }
            else //an already forked process, so we just continue where we stopped
            { 
                runningProcessStart = getClk();
                runningProcess->WaitTime = runningProcess->StartTime - runningProcess->ArrivalTime + runningProcess->RunningTime - runningProcess->RemainingTime;
                kill(runningProcess->ID, SIGCONT);
                runningProcess->Running = true;
                fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n", runningProcessStart, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
            }
        }

        else //I have a message I'm working on
        {
            if (runningProcess->RemainingTime < quantum)
            {
                
            }
            else if (runningProcess->RemainingTime = quantum)
            {

            }
            else if (runningProcess->RemainingTime > quantum)
            {

            }
        }
    }
}
