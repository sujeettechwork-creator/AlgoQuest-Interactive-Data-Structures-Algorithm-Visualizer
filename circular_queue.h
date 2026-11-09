#ifndef ALGOQUEST_CIRCULAR_QUEUE_H
#define ALGOQUEST_CIRCULAR_QUEUE_H

#define CIRCULAR_QUEUE_CAPACITY 8

typedef struct
{
    int items[CIRCULAR_QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
} CircularQueue;

void CircularQueueInit(CircularQueue *queue);
int CircularQueueIsEmpty(const CircularQueue *queue);
int CircularQueueIsFull(const CircularQueue *queue);
int CircularQueueEnqueue(CircularQueue *queue, int value);
int CircularQueueDequeue(CircularQueue *queue, int *removedValue);
int CircularQueuePeek(const CircularQueue *queue, int *frontValue);
int CircularQueueSize(const CircularQueue *queue);

#endif
