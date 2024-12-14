#include<headers.h>
#ifndef NODE_H 
#define NODE_H 

typedef struct priNode 
{
    processdata processobj; 
    struct priNode *next;
    int priority;
} priNode;
#endif