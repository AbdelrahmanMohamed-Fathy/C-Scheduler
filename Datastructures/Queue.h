#include <headers.h>
#include<Node.h>
#ifndef QUEUE_H
#define QUEUE_H

typedef struct Queue 
{
   Node *front;
   Node *rear;
}Queue;

Node* createNode(processdata p)
{
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node==NULL)
    {
        perror ("Memory allocation for new node has failed \n");
        exit (EXIT_FAILURE);
    }
    new_node->processobj = p;
    new_node->next = NULL;
    return new_node;
}

Queue* createQueue()
{
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

int isEmpty(Queue* q)
{

    if (q->front == NULL && q->rear == NULL)
    {
        return 1;
    }
    return 0;
}

void enqueue_proc(Queue* q, processdata process)
{
    Node* new_node = createNode(process);

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
}


void dequeue_proc(Queue* q)
{

    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    Node* temp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);
}
#endif