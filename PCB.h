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
    int lastend;

    // cpu stats
    int StartTime;
    int EndTime;
    int WaitTime;

    bool Running;

} PCB;