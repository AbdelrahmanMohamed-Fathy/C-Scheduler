#pragma once
#include "headers.h"
#include "DataStructures/MemTree.h"

typedef struct PCB
{
    // Process stats
    int generationID;
    int ID;
    int Priority;

    int ArrivalTime;
    int RunningTime;
    int RemainingTime;
    int lastend;

    bool Running;
    // Cpu stats
    int StartTime;
    int EndTime;
    int WaitTime;

    // MLFQ special
    int originalPriority;

    // Memory
    int Size;
    MemLocation Location;
} PCB;