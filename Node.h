#include<headers.h>
#ifndef NODE_H 
#define NODE_H // Define a node in the queue 

typedef struct Node 
{
    processData processobj; 
    struct Node *next;
} Node;
#endif