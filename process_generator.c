#include "headers.h"
#include "DataStructures/Queue.h"
#include "DataStructures/priQueue.h"

void clearResources(int);

int main(int argc, char *argv[])
{
    // creating the queue for processes
    Queue *proccesqueue;
    proccesqueue = CreateQueue();

    // pointer to handle the input file
    FILE *inputfile;

    // array of characters as we read the input file line by line
    char fileline[20];
    signal(SIGINT, clearResources);

    // input file processing
    inputfile = fopen("processes.txt", "r");
    if (inputfile == NULL)
    {
        perror("Error opening file");
        return -1;
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
        }
        else
        {
            // fprintf(stderr, "Error reading values from line: %s", fileline);
            perror("error in reading input file");
        }
    }

    // input file processing is done so its best to close the file
    fclose(inputfile);

    // TODO Initialization
    // 1. Read the input files.
    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
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
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    raise(SIGTERM);
}
