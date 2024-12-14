#include "../headers.h"
#ifndef PRINODE_H
#define PRINODE_H

typedef struct priNode
{
    processdata processobj;
    struct priNode *next;
    int priority;
} priNode;
#endif