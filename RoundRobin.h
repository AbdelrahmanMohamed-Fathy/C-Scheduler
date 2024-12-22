#include "headers.h"
#include "DataStructures/CircularQueue.h"
#include "PCB.h"

PCB *runningprocess = NULL;
PCB *newProcess;
CircQueue *RRqueue;
void RR(FILE *OutputFile, int ProcessMessageQueue, int quantum,cpuData* cpudata )
{
    int wait_time=0;
    //bool emptyqueueflag;
    bool currentprocessdone=true;
    bool messagesdone = false;
    bool firsttime =true;
    int time;
    msg RRmsg;
    RRqueue=CreatecircQueue();

    while (!isCircQueueEmpty(RRqueue) || !messagesdone || runningprocess)
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
            newProcess->lastend=0;
            newProcess->Running = false;
            CircEnqueue(RRqueue, &newProcess); 
        }
        
        if (!isCircQueueEmpty(RRqueue) || !currentprocessdone)
        {
            if(currentprocessdone){
                CircDequeue(RRqueue, &runningprocess);
                firsttime=true;
                currentprocessdone=false;
            }
            if (runningprocess->ID == -1)
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
                runningprocess->WaitTime = runningprocess->StartTime - runningprocess->ArrivalTime;
                runningprocess->Running = true;
            }
            if (runningprocess->RemainingTime <= quantum)
            {
                //if the process will finish in the current quantum
                kill(runningprocess->ID, SIGCONT);
                runningprocess->Running=true;
                if(currentprocessdone || firsttime){
                    if(firsttime)
                    {
                        time = getClk();
                    }
                    wait_time = (time + runningprocess->RemainingTime) - getClk();
                    if(runningprocess->RemainingTime == runningprocess->RunningTime){
                        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                        printf("At time %d process %d started arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                    }
                    else{
                        runningprocess->WaitTime+=getClk() - runningprocess->lastend;
                        fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                        printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                    }
                        
                }
                wait_time = (time + runningprocess->RemainingTime) - getClk();
                if (wait_time > 0)
                {
                    currentprocessdone=false;
                    firsttime=false;
                    continue;
                }
                else
                {
                    
                    currentprocessdone=true;
                    runningprocess->EndTime = getClk();
                    runningprocess->lastend = getClk();
                    runningprocess->RemainingTime = 0;
                    runningprocess->Running = false;
                    fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",runningprocess->EndTime , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime,runningprocess->EndTime - runningprocess->ArrivalTime,(runningprocess->EndTime - runningprocess->ArrivalTime) / (float)(runningprocess->RunningTime));
                    printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",runningprocess->EndTime , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime,runningprocess->EndTime - runningprocess->ArrivalTime,(runningprocess->EndTime - runningprocess->ArrivalTime) / (float)(runningprocess->RunningTime));
                    cpucalculations(cpudata, runningprocess);
                }   
                
            }
            else
            {   //if the process will not finish in the current quantum
                kill(runningprocess->ID, SIGCONT);
                runningprocess->Running=true;
                if(currentprocessdone || firsttime){
                    if(firsttime)
                    {
                        time = getClk();
                    }
                    wait_time = (time + quantum) - getClk();
                    if(runningprocess->RemainingTime == runningprocess->RunningTime){
                        fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                        printf("At time %d process %d started arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                    }
                    else
                    {
                        runningprocess->WaitTime+=getClk() - runningprocess->lastend;
                        fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                        printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);                        
                    }
                }
                wait_time = (time + quantum) - getClk();
                if (wait_time > 0)
                {
                    currentprocessdone=false;
                    firsttime=false;
                    continue;
                }
                else
                {
                    currentprocessdone=true;
                    runningprocess->RemainingTime -= quantum;
                    if(runningprocess->WaitTime<0)
                        runningprocess->WaitTime=0;
                    kill(runningprocess->ID, SIGSTOP);
                    runningprocess->Running=false;
                    runningprocess->lastend=getClk();
                    fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                    printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk() , runningprocess->generationID, runningprocess->ArrivalTime, runningprocess->RunningTime, runningprocess->RemainingTime,runningprocess->WaitTime);
                }   
                
            }
            if (runningprocess->RemainingTime > 0 && currentprocessdone)
            {   //if the process still has remaining time
                CircEnqueue(RRqueue, &runningprocess);
            }
            else if(currentprocessdone)
            {   //if the process has finished
                free(runningprocess);
                runningprocess = NULL;
            }
        }
    }
    printf("RR done");
    free(RRqueue);
}

void RRFree(){
    PCB* Dummy;
    if (RRqueue)
    {
        while (CircDequeue(RRqueue,&Dummy))
            free(Dummy);
        free(RRqueue);
    }
    if (runningprocess)
        free(runningprocess);
    if (newProcess)
        free(runningprocess);
    return;
}