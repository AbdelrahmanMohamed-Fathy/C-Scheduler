#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

#define NUM_QUEUES 11 // Number of priority levels
priQueue *queues[NUM_QUEUES];
int queueCounts[NUM_QUEUES];
PCB *runningProcess;
PCB *newProcess;



void MLFQ(FILE *OutputFile, int ProcessMessageQueue, int quantum, cpuData *perfdata)
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
            if (!queues[i]->front)
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
            newProcess->ArrivalTime = MLFQmsg.data.arrivaltime;
            newProcess->RunningTime = MLFQmsg.data.runningtime;
            newProcess->RemainingTime = MLFQmsg.data.runningtime;
            newProcess->StartTime = -1;
            newProcess->EndTime = -1;
            newProcess->WaitTime = 0;
            newProcess->Running = false;
            PriEnqueue(queues[newProcess->Priority], &newProcess, newProcess->ArrivalTime);
            fprintf(OutputFile, "#process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, newProcess->ArrivalTime, newProcess->RunningTime);
            newProcess = NULL;
            //printf("#process with id=%d arrived at clock=%d and running time=%d \n", newProcess->generationID, newProcess->ArrivalTime, newProcess->RunningTime);
        }
        
        if(runningProcess)
        {
            //handling already running process
            if (runningProcessStart + GivenQuantum - getClk() > 0)
                continue;

            else 
            {
                if (waitpid(runningProcess->ID, NULL, WNOHANG) == runningProcess->ID) //Since I expect it to happen, I wait for process termination
                {
                    runningProcess->EndTime = getClk();
                    runningProcess->RemainingTime -= GivenQuantum;
                    runningProcess->Running = false;
                    fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n", runningProcess->EndTime, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                    free(runningProcess);
                    runningProcess = NULL;
                    continue;
                }
                else
                {
                    int now = getClk();
                    PriEnqueue(queues[runningProcess->Priority],&runningProcess,now);
                    kill(runningProcess->ID,SIGTSTP);
                    runningProcess->Running = false;
                    runningProcess->RemainingTime -= GivenQuantum;
                    fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", now, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                    runningProcess=NULL;
                    continue;
                }
            }
        }
        else
        {
            //handling no running process
            bool Found = false;
            for (int i = CurrentQueue; i >= 0; i--)
            {
                queueCounts[i] = queues[i]->count;
                if(PriDequeue(queues[i],&runningProcess))
                {
                    Found = true;
                    break;
                }
            }
            if (!Found)
            {
                StartQueue = (StartQueue+1) % NUM_QUEUES;
                CurrentQueue = StartQueue;
                continue;
            }
            else
            {
                if (runningProcess->ID == -1)
                {
                    //handling process that never started before
                    runningProcess->ID = fork();
                    if (runningProcess->ID == 0)
                    {
                        //Child
                        char runtime[4];
                        sprintf(runtime, "%d", runningProcess->RunningTime);
                        execl("bin/process.out", "./process.out", runtime, NULL);
                    }
                    else
                    {
                        //setting up PCB for first start
                        runningProcess->StartTime = getClk();
                        runningProcessStart = runningProcess->StartTime;
                        runningProcess->Running = true;
                        runningProcess->WaitTime = runningProcess->StartTime - runningProcess->ArrivalTime;
                        GivenQuantum = (runningProcess->RemainingTime<=quantum) ? (runningProcess->RemainingTime) : (quantum); 
                        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n", runningProcess->StartTime, runningProcess->generationID, runningProcess->ArrivalTime, runningProcess->RunningTime, runningProcess->RemainingTime, runningProcess->WaitTime);
                    }
                }
                else
                {
                    //process already started but stopped
                    runningProcessStart = getClk();
                    kill(runningProcess->ID, SIGCONT);
                    runningProcess->Running = true;

                    runningProcess->WaitTime = runningProcess->StartTime - runningProcess->ArrivalTime + runningProcess->RunningTime - runningProcess->RemainingTime;
                    GivenQuantum = (runningProcess->RemainingTime>=quantum) ? (quantum) : (runningProcess->RemainingTime);
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
            while (PriDequeue(queues[i],&Dummy))
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