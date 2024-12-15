#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "pcb.h"

void RR(int ProcessMessageQueue, int quantum)
{
    int lastquantum = 0;
    int messagesdone;
    int time;
    msg RRmsg;
    CircQueue *RRqueue = CreatecircQueue();
    PCB *runningprocess;
    PCB *newProcess;
    while (!isCircQueueEmpty(RRqueue) || !messagesdone)
    {
        time = getClk();
        if (msgrcv(ProcessMessageQueue, &RRmsg, sizeof(struct msg), 1, IPC_NOWAIT) != -1)
        {
            messagesdone = true;
        }
        else if (msgrcv(ProcessMessageQueue, &RRmsg, sizeof(struct msg), 0, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = RRmsg.data.id;
            newProcess->ID = RRmsg.data.id; // this needs to change
            newProcess->Priority = RRmsg.data.priority;
            newProcess->ArrivalTime = RRmsg.data.arrivaltime;
            newProcess->RunningTime = RRmsg.data.runningtime;
            newProcess->RemainingTime = RRmsg.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            CircEnqueue(RRqueue, newProcess);
        }

        if (!isCircQueueEmpty(RRqueue))
        {
            CircDequeue(RRqueue, runningprocess);
            if (runningprocess->StartTime == -1)
            {
                runningprocess->StartTime = time;
            }
            if (runningprocess->RemainingTime <= quantum)
            {
                while (getClk() < (time + runningprocess->RemainingTime))
                    ;
                runningprocess->RunningTime += runningprocess->RemainingTime;
                runningprocess->RemainingTime = 0;
                runningprocess->EndTime = getClk();
                printf("process with id=%d and runningtime=%d finished at %d", runningprocess->ID, runningprocess->RunningTime, runningprocess->EndTime);
            }
            else
            {
                while (getClk() < (time + quantum))
                    ;
                runningprocess->RemainingTime -= quantum;
                runningprocess->RunningTime += quantum;
                printf("process with id=%d remaining time=%d clock=%d ", runningprocess->ID, runningprocess->RemainingTime, getClk());
                CircEnqueue(RRqueue, runningprocess);
            }
        }
    }
    printf("RR done");
}