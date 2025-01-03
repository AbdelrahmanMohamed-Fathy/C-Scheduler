#pragma once
#include "../headers.h"
#include "priNode.h"

typedef struct priQueue
{
    priNode *front;
    priNode *rear;
    int count;
} priQueue;

priNode *CreatePriNode(PCB *p)
{
    priNode *new_node = (priNode *)malloc(sizeof(priNode));
    if (new_node == NULL)
    {
        perror("Memory allocation for new node has failed \n");
        exit(EXIT_FAILURE);
    }
    new_node->processobj = p;
    new_node->priority = p->Priority;
    new_node->next = NULL;

    return new_node;
}

priQueue *CreatePriQueue()
{
    priQueue *q = (priQueue *)malloc(sizeof(priQueue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

bool isPriQueueEmpty(priQueue *q)
{
    if (q->front == NULL && q->rear == NULL)
    {
        return true;
    }
    return false;
}

void PriEnqueue(priQueue *q, PCB **process, int priority)
{
    priNode *new_node = CreatePriNode(*process);

    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
        q->count++;
        return;
    }
    priNode *current = q->front;
    if (new_node->priority < current->priority)
    {
        new_node->next = q->front;
        q->front = new_node;
        q->count++;
        return;
    }
    priNode *previous = q->front;
    current = current->next;
    while (current)
    {
        if (new_node->priority < current->priority)
        {
            new_node->next = current;
            previous->next = new_node;
            q->count++;
            return;
        }
        else
        {
            current = current->next;
            previous = previous->next;
        }
    }
    q->rear->next = new_node;
    q->rear = new_node;
    q->count++;
}

bool PriDequeue(priQueue *q, PCB **data)
{
    if (!q->front)
    {
        data = NULL;
        return false;
    }
    priNode *Current = q->front;
    q->front = q->front->next;
    *data = Current->processobj;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    free(Current);
    q->count--;
    return true;
}