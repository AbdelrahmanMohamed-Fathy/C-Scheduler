#include "headers.h"

/* Modify this file as needed*/

int main(int agrc, char *argv[])
{
    initClk();
    int remainingtime = atoi(argv[1]);

    // TODO The process needs to get the remaining time from somewhere
    int time = getClk();
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
