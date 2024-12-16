#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

void RR(FILE * OutputFile, int ProcessMessageQueue, int quantum)
{
    int lastquantum = 0;
    int wait_time;
    bool messagesdone = false;
    int time;
    msg RRmsg;
    CircQueue *RRqueue = CreatecircQueue();
    PCB *runningprocess = NULL;
    PCB *newProcess;
    while (!isCircQueueEmpty(RRqueue) || !messagesdone)
    {
        lastquantum = getClk();
        if (msgrcv(ProcessMessageQueue, &RRmsg, sizeof(msg), 20, IPC_NOWAIT) != -1)
        {
            messagesdone = true;
        }
        else if (msgrcv(ProcessMessageQueue, &RRmsg, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = RRmsg.data.id;
            newProcess->ID = -1; 
            newProcess->Priority = RRmsg.data.priority;
            newProcess->ArrivalTime = RRmsg.data.arrivaltime;
            newProcess->RunningTime = RRmsg.data.runningtime;
            newProcess->RemainingTime = RRmsg.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->Running = false;
            CircEnqueue(RRqueue, &newProcess);
            fprintf(OutputFile, "process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, getClk(), newProcess->RunningTime);
        }

        if (!isCircQueueEmpty(RRqueue))
        {
            CircDequeue(RRqueue, &runningprocess);
            if (runningprocess->StartTime == -1)
            {
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
                runningprocess->Running = true;
            }
            if (runningprocess->RemainingTime <= quantum)
            {
                //if the process will finish in the current quantum
                kill(runningprocess->ID, SIGCONT);
                time = getClk();
                wait_time = (time + runningprocess->RemainingTime) - getClk();
                if (wait_time > 0)
                    sleep(wait_time);
                runningprocess->EndTime = getClk();
                runningprocess->RemainingTime = 0;
                runningprocess->Running = false;
                fprintf(OutputFile, "process with id=%d and runningtime=%d finished at %d \n", runningprocess->generationID, runningprocess->RunningTime, runningprocess->EndTime);
            }
            else
            {
                kill(runningprocess->ID, SIGCONT);
                time = getClk();
                wait_time = (time + runningprocess->RemainingTime) - getClk();
                if (wait_time > 0)
                    sleep(wait_time);
                runningprocess->RemainingTime -= quantum;
                kill(runningprocess->ID, SIGSTOP);
                fprintf(OutputFile, "process with id=%d remaining time=%d clock=%d \n", runningprocess->generationID, runningprocess->RemainingTime, getClk());
            }
            if (runningprocess->RemainingTime > 0)
            {
                CircEnqueue(RRqueue, &runningprocess);
            }
            else
            {
                free(runningprocess);
            }
        }
    }
    printf("RR done");
    free(RRqueue);
}


