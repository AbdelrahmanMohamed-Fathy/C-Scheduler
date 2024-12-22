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
MemTree *MemoryTree;

// TODO: implement the scheduler.
// TODO: upon termination release the clock resources.

void printMem(FILE *file, MemTree *Tree, int Size)
{
    MemLocation *location = TreeAllocate(Tree, Size);
    fprintf(file, "Allocated at %d ~ %d\n", location->Start, location->End);
}

int main(int argc, char *argv[])
{

    signal(SIGINT, clearResources);
    initClk();
    SchedulingAlgorithm = atoi(argv[1]);
    int Quantum = atoi(argv[2]);
    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    cpuData PreformanceData;
    PreformanceData.Count = 0;
    PreformanceData.TotalWaitTime = 0;
    PreformanceData.totalRunTime = 0;
    PreformanceData.totalWTA = 0;
    MemoryTree = CreateMemTree();
    FILE *perffile = fopen("scheduler.perf", "w");
    FILE *OutputFile = fopen("scheduler.log", "w");
    FILE *MemFile = fopen("memory.log", "w");
    switch (SchedulingAlgorithm)
    {
    case Shortest_Job_First:
        fprintf(OutputFile, "#SJF:\n");
        SJF(OutputFile, ProcessMessageQueue, &PreformanceData);
        break;
    case Premptive_Highest_Priority_First:
        fprintf(OutputFile, "#HPF:\n");
        HPF(OutputFile, ProcessMessageQueue, &PreformanceData);
        break;
    case Round_Robin:
        fprintf(OutputFile, "#RR:\n");
        RR(OutputFile, ProcessMessageQueue, Quantum, &PreformanceData);
        break;
    case Multiple_Level_Feedback_Loop:
        fprintf(OutputFile, "#MLFP:\n");
        MLFQ(OutputFile, ProcessMessageQueue, Quantum, &PreformanceData);
        break;
    }
    fprintf(perffile, "CPU utilization = %.2f%%  \nAvg WTA =  %f \nAvg Waiting = %f \n", PreformanceData.util, PreformanceData.AvgWeightedTurnaroundTime, PreformanceData.AvgWaitTime);

    fclose(OutputFile);
    fclose(perffile);
    fclose(MemFile);
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
    DeleteTree(MemoryTree);
    exit(1);
}