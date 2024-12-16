typedef struct cpuData
{
    int Count;

    int TotalWaitTime;
    float AvgWaitTime;

    float AvgWeightedTurnaroundTime;

} cpuData;

void meow(cpuData* cpudata, PCB* process){
    cpudata->Count++;
    cpudata->TotalWaitTime += process->WaitTime;
    cpudata->AvgWaitTime = cpudata->TotalWaitTime / cpudata->Count;
    meow=(process->EndTime - process->ArrivalTime)/process->RunningTime;
    cpudata->AvgWeightedTurnaroundTime = (cpudata->AvgWeightedTurnaroundTime + meow)/cpuData->Count;
}