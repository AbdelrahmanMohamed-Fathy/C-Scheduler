#pragma once
#include "../headers.h"
#include "Node.h"

typedef struct Queue
{
    Node *front;
    Node *rear;
    int count;
} Queue;

Node *CreateNode(processData *p)
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


Queue *CreateQueue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

bool isQueueEmpty(Queue *q)
{

    if (q->front == NULL && q->rear == NULL)
    {
        return true;
    }
    return false;
}

void Enqueue(Queue *q, processData **process)
{
    Node *new_node = CreateNode(*process);

    if (q->rear == NULL)
    {
        q->front = new_node;
        q->rear = new_node;
    }
    else
    {
        q->rear->next = new_node;
        q->rear = new_node;
    }
    q->count++;
}

bool Dequeue(Queue *q, processData **data)
{
    if (!q->front)
    {
        data = NULL;
        return false;
    }
    Node *current = q->front;
    q->front = q->front->next;
    *data = current->processobj;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    free(current);
    q->count--;
    return true;
}