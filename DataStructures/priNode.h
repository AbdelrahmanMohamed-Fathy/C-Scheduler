#pragma once
#include "../headers.h"
#include "../pcb.h"

typedef struct priNode
{
    PCB *processobj;
    struct priNode *next;
    int priority;
} priNode;