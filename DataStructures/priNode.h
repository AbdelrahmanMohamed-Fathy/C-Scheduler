#pragma once
#include "../headers.h"

typedef struct priNode
{
    processData *processobj;
    struct priNode *next;
    int priority;
} priNode;