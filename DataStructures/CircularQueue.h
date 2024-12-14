#pragma once
#include "../headers.h"
#include "Node.h"

typedef struct CircQueue
{
    Node *front;
    Node *rear;
    int count;
} CircQueue;

Node *CreatecircNode(processData *p)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        perror("Memory allocation for new node has failed \n");
        exit(EXIT_FAILURE);
    }
    new_node->processobj = p;
    new_node->next = NULL;
    return new_node;
}

CircQueue *CreatecircQueue()
{
    CircQueue *q = (CircQueue *)malloc(sizeof(CircQueue));
    q->front = NULL;
    q->rear = NULL;
    q->rear->next=q->front;
    return q;
}

bool isCircQueueEmpty(CircQueue *q)
{

    if (q->front == NULL && q->rear == NULL)
    {
        return true;
    }
    return false;
}

void CircEnqueue(CircQueue *q, processData *process)
{
    Node *new_node = CreatecircNode(process);

    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
        q->rear->next = q->front;
    }
    else
    {
        q->rear->next = new_node;
        q->rear = new_node;
        q->rear->next=q->front;
    }
    q->count++;
}

bool CircDequeue(CircQueue *q, processData *data)
{
    if (!q->front)
    {
        data = NULL;
        return false;
    }
    Node *current = q->front;
    q->front = q->front->next;
    q->rear->next=q->front;
    data = current->processobj;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    free(current);
    q->count--;
    return true;
}