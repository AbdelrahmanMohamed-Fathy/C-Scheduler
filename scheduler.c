#include "DataStructures/priQueue.h"
#include "headers.h"
#include "cpuData.h"
#include "pcb.h"
#include "hpf.h"
#include "RoundRobin.h"

void clearResources(int signum);

int ProcessMessageQueue;

// TODO: implement the scheduler.
// TODO: upon termination release the clock resources.
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    initClk();
    Algorithm SchedulingAlgorithm = atoi(argv[1]);
    int Quantum = atoi(argv[2]);
    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);

    switch (SchedulingAlgorithm)
    {
    case Shortest_Job_First:
        // SJF();
        break;
    case Premptive_Highest_Priority_First:
        HPF(ProcessMessageQueue);
        break;
    case Round_Robin:
        RR(ProcessMessageQueue, Quantum);
        break;
    case Multiple_Level_Feedback_Loop:
        break;
    }

    printf("scheduler terminating normally.\n");
    destroyClk(true);
    clearResources(0);
}

void clearResources(int signum)
{
    msgctl(ProcessMessageQueue, IPC_RMID, NULL);
    exit(1);
}

