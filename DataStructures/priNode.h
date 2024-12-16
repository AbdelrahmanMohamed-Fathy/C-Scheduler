#pragma once
#include "../headers.h"
#include "../PCB.h"

typedef struct priNode
{
    PCB *processobj;
    struct priNode *next;
    int priority;
} priNode;