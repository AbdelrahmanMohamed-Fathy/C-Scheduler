#include "headers.h"

/* Modify this file as needed*/

void cont(int signum);

int time;

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT, cont);
    int remainingtime = atoi(argv[1]);

    // TODO The process needs to get the remaining time from somewhere
    time = getClk();
    while (remainingtime > 0)
    {
        while (time == getClk())
            continue;
        remainingtime--;
        time = getClk();
    }

    destroyClk(false);

    return 0;
}

void cont(int signum)
{
    time = getClk();
}