#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

void RR(FILE * OutputFile, int ProcessMessageQueue, int quantum)
{
    int lastquantum = 0;
    bool messagesdone=false;
    int time;
    msg RRmsg;
    CircQueue *RRqueue = CreatecircQueue();
    PCB *runningprocess;
    PCB *newProcess;
    while (!isCircQueueEmpty(RRqueue) || !messagesdone)
    {
        lastquantum=getClk();
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
            fprintf(OutputFile,"process with id=%d arrived at clock=%d and running time=%d \n",newProcess->generationID,getClk(),newProcess->RunningTime);
        }

        if (!isCircQueueEmpty(RRqueue)) //if there's a process to run (will always be true after the first process arrives)
        {
            CircDequeue(RRqueue, &runningprocess);
            if (runningprocess->StartTime == -1)
            {
                //if its the first time to run the process
                runningprocess->ID=fork();
                if (runningprocess->ID == 0)
                {
                    char runtime[4];
                    sprintf(runtime, "%d", runningprocess->RunningTime);
                    execl("bin/process.out", "./process.out", runtime, NULL);
                }
                runningprocess->StartTime = getClk();    
                runningprocess->Running = true;
            }
            
            if (runningprocess->RemainingTime <= quantum)
            {
                //if the process will finish in the current quantum
                kill(runningprocess->ID, SIGCONT);
                time = getClk();
                while (getClk() < (time + runningprocess->RemainingTime));
                runningprocess->RemainingTime = 0;
                runningprocess->Running = false;
                runningprocess->EndTime = getClk();
                kill(runningprocess->ID, SIGSTOP);
                fprintf(OutputFile, "process with id=%d and runningtime=%d finished at %d \n", runningprocess->generationID, runningprocess->RunningTime, runningprocess->EndTime);
                //free(runningprocess);
            }
            else
            {
                //if the process will not finish in the current quantum                
                kill(runningprocess->ID, SIGCONT);
                time =getClk();
                while (getClk() < (time + quantum));
                runningprocess->RemainingTime -= quantum;
                kill(runningprocess->ID, SIGSTOP);
                fprintf(OutputFile,"process with id=%d remaining time=%d clock=%d \n", runningprocess->generationID, runningprocess->RemainingTime, getClk());
                
            }
            while(RRqueue->count==0 && runningprocess->Running){
                if (runningprocess->RemainingTime <= quantum)
                {
                    //if the process will finish in the current quantum
                    kill(runningprocess->ID, SIGCONT);
                    time = getClk();
                    while (getClk() < (time + runningprocess->RemainingTime));
                    runningprocess->RemainingTime = 0;
                    runningprocess->Running = false;
                    runningprocess->EndTime = getClk();
                    kill(runningprocess->ID, SIGSTOP);
                    fprintf(OutputFile, "process with id=%d and runningtime=%d finished at %d \n", runningprocess->generationID, runningprocess->RunningTime, runningprocess->EndTime);
                    //free(runningprocess);
                }
                else
                {
                    //if the process will not finish in the current quantum                
                    kill(runningprocess->ID, SIGCONT);
                    time =getClk();
                    while (getClk() < (time + quantum));
                    runningprocess->RemainingTime -= quantum;
                    kill(runningprocess->ID, SIGSTOP);
                    fprintf(OutputFile, "process with id=%d remaining time=%d clock=%d \n", runningprocess->generationID, runningprocess->RemainingTime, getClk());
                    
                }

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
                    fprintf(OutputFile,"process with id=%d arrived at clock=%d and running time=%d \n",newProcess->generationID,getClk(),newProcess->RunningTime);
                }
                if(!isCircQueueEmpty(RRqueue)){
                    CircEnqueue(RRqueue,&runningprocess);
                    CircDequeue(RRqueue,&runningprocess);
                }
                lastquantum=getClk();
            }
        }
    }
    printf("RR done");
    free(RRqueue);
}



