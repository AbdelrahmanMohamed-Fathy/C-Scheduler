typedef struct cpuData
{
    float util;
    int Count;

    int totalRunTime;
    int TotalWaitTime;
    float AvgWaitTime;

    float totalWTA;
    float AvgWeightedTurnaroundTime;

} cpuData;

void cpucalculations(cpuData* cpudata, PCB* process){
    cpudata->Count++;
    cpudata->TotalWaitTime += process->WaitTime;
    cpudata->totalRunTime += process->RunningTime;
    cpudata->AvgWaitTime = cpudata->TotalWaitTime / cpudata->Count;
    cpudata->totalWTA +=(process->EndTime - process->ArrivalTime)/(float)process->RunningTime;
    cpudata->AvgWeightedTurnaroundTime = cpudata->totalWTA/cpudata->Count;
    cpudata->util = (cpudata->totalRunTime)/(float)(process->EndTime)*100;
}