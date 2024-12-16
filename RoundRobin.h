#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

void RR(FILE * OutputFile, int ProcessMessageQueue, int quantum )
{
    int wait_time;
    bool messagesdone = false;
    int time;
    msg RRmsg;
    CircQueue *RRqueue = CreatecircQueue();
    PCB *runningprocess = NULL;
    PCB *newProcess;
    cpuData* cpudata;
    while (!isCircQueueEmpty(RRqueue) || !messagesdone)
    {
        if (msgrcv(ProcessMessageQueue, &RRmsg, sizeof(msg), 20, IPC_NOWAIT) != -1)
        {
            messagesdone = true;
        }
        else if(msgrcv(ProcessMessageQueue, &RRmsg, sizeof(msg), 1, IPC_NOWAIT) != -1)
        {
            newProcess = (PCB *)malloc(sizeof(PCB));
            newProcess->generationID = RRmsg.data.id;
            newProcess->ID = -1; //not created yet
            newProcess->Priority = RRmsg.data.priority;
            newProcess->ArrivalTime = RRmsg.data.arrivaltime;
            newProcess->RunningTime = RRmsg.data.runningtime;
            newProcess->RemainingTime = RRmsg.data.runningtime;
            newProcess->StartTime = -1; //not started yet
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->Running = false;
            CircEnqueue(RRqueue, &newProcess);
        }

        if (!isCircQueueEmpty(RRqueue))
        {
            CircDequeue(RRqueue, &runningprocess);
            if (runningprocess->StartTime == -1)
            {   // if its the first time for the process to run
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
                runningprocess->WaitTime = runningprocess->EndTime - runningprocess->StartTime - runningprocess->RunningTime + runningprocess->RemainingTime;
                meow(cpudata, runningprocess);
                fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %d\n",runningprocess->EndTime , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime,runningprocess->EndTime - runningprocess->ArrivalTime,(runningprocess->EndTime - runningprocess->ArrivalTime) / runningprocess->RunningTime);
            }
            else
            {   //if the process will not finish in the current quantum
                kill(runningprocess->ID, SIGCONT);
                time = getClk();
                if(runningprocess->RemainingTime == runningprocess->RunningTime)
                    fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",time , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                else
                    fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",time , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                wait_time = (time + quantum) - getClk();
                if (wait_time > 0)
                    sleep(wait_time);
                runningprocess->RemainingTime -= quantum;
                runningprocess->WaitTime = getClk() - runningprocess->StartTime - runningprocess->RunningTime + runningprocess->RemainingTime;
                kill(runningprocess->ID, SIGSTOP);
                fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
            }
            if (runningprocess->RemainingTime > 0)
            {   //if the process still has remaining time
                CircEnqueue(RRqueue, &runningprocess);
            }
            else
            {   //if the process has finished
                free(runningprocess);
            }
        }
    }
    printf("RR done");
    free(RRqueue);
}


