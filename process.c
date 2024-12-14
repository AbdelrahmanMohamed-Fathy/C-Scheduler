#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();

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
