#include <errno.h>
#include <string.h>
#include "headers.h"
#include "DataStructures/Queue.h"
#include "DataStructures/priQueue.h"

void clearResources(int);

Queue *proccesqueue;
bool DebugMode = false;

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    proccesqueue = CreateQueue();

    if (argc > 1)
    {
        DebugMode = atoi(argv[1]);
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
    // reading the input file line by line and soring the parameters of each process in processobj
    // then enqueuing the process in the process queue
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
            Enqueue(proccesqueue, processobj);
            if (DebugMode == true)
                printf("Process entering queue:\nID:%d\nArrival Time:%d\nRuntime:%d\nPriority:%d\n", processobj->id, processobj->arrivaltime, processobj->runningtime, processobj->priority);
        }
        else
        {
            perror("error in reading input file");
        }
    }
    fclose(inputfile);

    // TODO Initialization
    // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    pid_t clock = fork();
    if (clock == 0)
    {
        execl("bin/clk.out", "./clk.out", NULL);
    }
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock.
    initClk();
    // To get time use this function.
    int x = getClk();
    printf("Current Time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    processData *data;
    while (Dequeue(proccesqueue, data))
    {
        free(data);
    }
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    processData *data;
    while (Dequeue(proccesqueue, data))
    {
        free(data);
    }
    exit(1);
}
