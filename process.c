#include "headers.h"
#include <stdatomic.h>

/* Modify this file as needed*/

void cont(int signum);

void stop(int signum);

int time;

atomic_int remainingtime;

int KeepLooping;

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGCONT, cont);
    // signal(SIGTSTP, stop);
    remainingtime = atoi(argv[1]);

    // TODO The process needs to get the remaining time from somewhere
    time = getClk();
    while (remainingtime > 0)
    {
        while (KeepLooping == 0)
            KeepLooping = time - getClk();

        atomic_fetch_sub(&remainingtime, 1);
        time = getClk();
        KeepLooping = 0;
        printf("%d\n", remainingtime);
    }

    destroyClk(false);
    return 0;
}

void cont(int signum)
{
    time = getClk();
}

// void stop(int signum)
//{
//     printf("%d %d %d\n", time, getClk(), KeepLooping);
//     // if (remainingtime != prevTime)
//     //   remainingtime--;
//     raise(SIGSTOP);
// }