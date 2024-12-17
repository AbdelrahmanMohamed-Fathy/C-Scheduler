#include "DataStructures/priQueue.h"
#include "headers.h"
#include "cpuData.h"
#include "SJF.h"
#include "HPF.h"
#include "RoundRobin.h"
#include "MLFQ.h"

void clearResources(int signum);

int ProcessMessageQueue;
Algorithm SchedulingAlgorithm;
// TODO: implement the scheduler.
// TODO: upon termination release the clock resources.
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    initClk();
    SchedulingAlgorithm = atoi(argv[1]);
    int Quantum = atoi(argv[2]);
    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    cpuData PreformanceData;

    FILE *OutputFile = fopen("scheduler.log", "w");
    switch (SchedulingAlgorithm)
    {
    case Shortest_Job_First:
        fprintf(OutputFile, "#SJF:\n");
        SJF(OutputFile,ProcessMessageQueue);
        break;
    case Premptive_Highest_Priority_First:
        fprintf(OutputFile, "#HPF:\n");
        HPF(OutputFile, ProcessMessageQueue);
        break;
    case Round_Robin:
        fprintf(OutputFile, "#RR:\n");
        RR(OutputFile, ProcessMessageQueue, Quantum);
        break;
    case Multiple_Level_Feedback_Loop:
        fprintf(OutputFile, "#MLFP:\n");
        MLFQ(OutputFile, ProcessMessageQueue, Quantum);
        break;
    }

    fclose(OutputFile);
    printf("scheduler terminating normally.\n");
    destroyClk(false);
    clearResources(0);
}

void clearResources(int signum)
{
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    switch (SchedulingAlgorithm)
    {
    case Shortest_Job_First:
        SJFFree();
        break;
    case Premptive_Highest_Priority_First:
        HPFFree();
        break;
    case Round_Robin:
        RRFree();
        break;
    case Multiple_Level_Feedback_Loop:
        MLFQFree();
        break;
    }
    exit(1);
}
