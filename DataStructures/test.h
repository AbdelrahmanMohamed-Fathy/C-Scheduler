#include <stdio.h>
#include "CircularQueue.h"

int main()
{
    CircQueue *queue = CreatecircQueue();
    PCB p1 = {1, 0, 0, 0, 0, 0, 0, 0, false}; // Sample PCB
    PCB p2 = {2, 0, 0, 0, 0, 0, 0, 0, false};

    PCB *process1 = &p1;
    PCB *process2 = &p2;

    // Test enqueue
    CircEnqueue(queue, &process1);
    CircEnqueue(queue, &process2);

    // Test dequeue
    PCB *deq_process;
    CircDequeue(queue, &deq_process);
    printf("Dequeued process ID: %d\n", deq_process->generationID);

    CircDequeue(queue, &deq_process);
    printf("Dequeued process ID: %d\n", deq_process->generationID);

    // Test empty queue
    if (isCircQueueEmpty(queue))
        printf("Queue is empty\n");

    free(queue);
    return 0;
}