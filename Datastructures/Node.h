#include<headers.h>
#ifndef NODE_H 
#define NODE_H  

typedef struct Node 
{
    processdata processobj; 
    struct Node *next;
} Node;
#endif