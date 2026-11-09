#ifndef ALGOQUEST_QUEUE_H
#define ALGOQUEST_QUEUE_H

#define QUEUE_CAPACITY 8

typedef struct
{
    int items[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
} Queue;

void QueueInit(Queue *queue);
int QueueIsEmpty(const Queue *queue);
int QueueIsFull(const Queue *queue);
int QueueEnqueue(Queue *queue, int value);
int QueueDequeue(Queue *queue, int *removedValue);
int QueuePeek(const Queue *queue, int *frontValue);
int QueueSize(const Queue *queue);

#endif
