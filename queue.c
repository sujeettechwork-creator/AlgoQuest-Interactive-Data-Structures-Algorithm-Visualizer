#include "queue.h"

void QueueInit(Queue *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
}

int QueueIsEmpty(const Queue *queue)
{
    return queue->count == 0;
}

int QueueIsFull(const Queue *queue)
{
    return queue->count == QUEUE_CAPACITY;
}

int QueueEnqueue(Queue *queue, int value)
{
    if (QueueIsFull(queue))
        return 0;

    queue->rear++;
    queue->items[queue->rear] = value;
    queue->count++;
    return 1;
}

int QueueDequeue(Queue *queue, int *removedValue)
{
    if (QueueIsEmpty(queue))
        return 0;

    *removedValue = queue->items[queue->front];
    queue->front++;
    queue->count--;

    if (queue->count == 0)
    {
        queue->front = 0;
        queue->rear = -1;
    }

    return 1;
}

int QueuePeek(const Queue *queue, int *frontValue)
{
    if (QueueIsEmpty(queue))
        return 0;

    *frontValue = queue->items[queue->front];
    return 1;
}

int QueueSize(const Queue *queue)
{
    return queue->count;
}
