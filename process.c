#include "headers.h"

/* Modify this file as needed*/

void cont(int signum);

void stop(int signum);

int time;

int prevTime = 0;

int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT, cont);
    signal(SIGTSTP, stop);
    remainingtime = atoi(argv[1]);

    // TODO The process needs to get the remaining time from somewhere
    time = getClk();
    while (remainingtime > 0)
    {
        prevTime = remainingtime;
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

void stop(int signum)
{
    printf("%d %d\n", remainingtime, prevTime);
    if (remainingtime != prevTime)
        remainingtime--;
    raise(SIGSTOP);
}