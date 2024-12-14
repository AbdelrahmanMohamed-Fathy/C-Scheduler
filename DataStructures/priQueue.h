#include "../headers.h"
#include "priNode.h"
#ifndef PRIQUEUE_H
#define PRIQUEUE_H

typedef struct priQueue
{
    priNode *front;
    priNode *rear;
} priQueue;

priNode *createpriNode(processdata p)
{
    priNode *new_node = (priNode *)malloc(sizeof(priNode));
    if (new_node == NULL)
    {
        perror("Memory allocation for new node has failed \n");
        exit(EXIT_FAILURE);
    }
    new_node->processobj = p;
    new_node->priority = p.priority;
    new_node->next = NULL;

    return new_node;
}

priQueue *createpriQueue()
{
    priQueue *q = (priQueue *)malloc(sizeof(priQueue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

bool isPriQueueEmpty(priQueue *q)
{
    if (q->front == NULL && q->rear == NULL)
    {
        return 1;
    }
    return 0;
}

void prienqueue_poc(priQueue *q, processdata process, int priority)
{
    priNode *new_node = createpriNode(process);

    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
    }
    priNode *temp = q->front;
    priNode *temp2 = q->front;
    if (new_node->priority < temp->priority)
    {
        new_node->next = q->front;
        q->front = new_node;
        temp = temp->next;
    }
    else
    {
        temp = temp->next;
        while (temp)
        {
            if (new_node->priority < temp->priority)
            {
                new_node->next = temp;
                temp2->next = new_node;
                temp = temp->next;
                temp2 = temp2->next;
                break;
            }
            else
            {
                temp = temp->next;
                temp2 = temp2->next;
            }
        }
        q->rear->next = new_node;
        q->rear = new_node;
    }
}

void pridequeue_proc(priQueue *q)
{

    if (isPriQueueEmpty(q))
    {
        printf("Queue is empty\n");
        return;
    }

    priNode *temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
}
#endif