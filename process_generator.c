#include <errno.h>
#include <string.h>
#include "headers.h"
#include "DataStructures/Queue.h"

void clearResources(int);

Queue *proccesqueue;
bool DebugMode = false;
Algorithm SchedulingAlgorithm = Shortest_Job_First;
int Quantum = 2;
int ProcessMessageQueue = -1;
pid_t scheduler = -1;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    char *FilePath;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
            DebugMode = true;
        else if (strcmp(argv[i], "-sch") == 0)
            SchedulingAlgorithm = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "-q") == 0)
            Quantum = atoi(argv[i + 1]);
        else
            FilePath = argv[i];
    }

    FILE *inputfile;

    char fileline[20];

    errno = 0;
    inputfile = fopen("processes.txt", "r");
    if (errno != 0)
    {
        fprintf(stderr, "Error opening file. %s\n", strerror(errno));
        exit(1);
    }

    // TODO Initialization
    // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.initClk();
    // 5. Create a data structure for processes and provide it with its parameters.

    // reading the input file line by line and soring the parameters of each process in processobj
    // then enqueuing the process in the process queue
    proccesqueue = CreateQueue();
    processData *processobj;
    while (fgets(fileline, sizeof(fileline), inputfile))
    {
        if (fileline[0] == '#')
        {
            continue;
        }
        processobj = (processData *)malloc(sizeof(processData));
        if (sscanf(fileline, "%d\t%d\t\t%d\t\t%d\n", &processobj->id, &processobj->arrivaltime, &processobj->runningtime, &processobj->priority) == 4)
        {
            Enqueue(proccesqueue, &processobj);
            if (DebugMode == true)
                printf("Process entering queue:\nID:%d\nArrival Time:%d\nRuntime:%d\nPriority:%d\n", processobj->id, processobj->arrivaltime, processobj->runningtime, processobj->priority);
        }
        else
        {
            free(processobj);
        }
    }
    fclose(inputfile);

    pid_t clock = fork();
    if (clock == 0)
    {
        execl("bin/clk.out", "./clk.out", NULL);
    }
    initClk();
    pid_t scheduler = fork();
    if (scheduler == 0)
    {
        char algo[4];
        char quant[4];
        sprintf(algo, "%d", SchedulingAlgorithm);
        sprintf(quant, "%d", Quantum);
        execl("bin/scheduler.out", "./scheduler.out", algo, quant, NULL);
    }
    //  To get time use this function.
    int x = getClk();
    printf("Current Time is %d\n", x);

    // TODO Generation Main Loop
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources

    ProcessMessageQueue = msgget(MSGKEY, IPC_CREAT | 0666);
    if (ProcessMessageQueue == -1)
    {
        printf("Error in creating ready queue \n");
        destroyClk(true);
    }
    if (DebugMode)
        printf("sending data to scheduler.\n");
    processData *data;
    msg SchedulerMessage;
    while (Dequeue(proccesqueue, &data))
    {
        if (DebugMode)
            printf("retrieved data with id: %d\n", data->id);

        while (data->arrivaltime > getClk())
            continue;

        SchedulerMessage.mtype = 1;
        memcpy(&SchedulerMessage.data, data, sizeof(processData));
        if (msgsnd(ProcessMessageQueue, &SchedulerMessage, sizeof(SchedulerMessage), !IPC_NOWAIT) != -1)
        {
            if (DebugMode)
                printf("Message sent succesfully for process with id: %d\n", SchedulerMessage.data.id);
        }
        free(data);
    }
    // Terminating message
    SchedulerMessage.mtype = 20;
    msgsnd(ProcessMessageQueue, &SchedulerMessage, sizeof(SchedulerMessage), !IPC_NOWAIT);
    if (DebugMode)
                printf("Termination Message Sent");

    if (DebugMode)
        printf("generator terminating normally.\n");
    destroyClk(false);
    clearResources(0);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    processData *data;
    while (Dequeue(proccesqueue, &data) == 1)
    {
        free(data);
    }
    if (scheduler == -1)
        destroyClk(true);
    if (proccesqueue)
        free(proccesqueue);
    exit(1);
}
