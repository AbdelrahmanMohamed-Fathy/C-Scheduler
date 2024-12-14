#pragma once
typedef struct PCB
{
    // process stats
    int ID;
    int Priority;

    int ArrivalTime;
    int RunningTime;
    int RemainingTime;

    // cpu stats
    int StartTime;
    int EndTime;
    int WaitTime;
} PCB;