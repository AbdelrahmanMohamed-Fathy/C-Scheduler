#pragma once
#include "../headers.h"
#include "../pcb.h"

typedef struct Node
{
    processData *processobj;
    struct Node *next;
} Node;

typedef struct PCBNode
{
    PCB* processobj;
    struct PCBNode *next;
} PCBNode;