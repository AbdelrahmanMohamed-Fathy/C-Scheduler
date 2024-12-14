#pragma once
#include "../headers.h"

typedef struct Node
{
    processData *processobj;
    struct Node *next;
} Node;