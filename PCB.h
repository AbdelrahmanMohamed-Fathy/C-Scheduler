#pragma once
#include "headers.h"

typedef struct PCB
{
    // process stats
    int generationID;
    int ID;
    int Priority;

    int ArrivalTime;
    int RunningTime;
    int RemainingTime;

    // cpu stats
    int StartTime;
    int EndTime;
    int WaitTime;

    //MLFQ special
    int originalPriority;

    bool Running;

} PCB;