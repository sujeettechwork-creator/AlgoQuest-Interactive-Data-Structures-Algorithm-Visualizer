#include "circular_queue.h"

void CircularQueueInit(CircularQueue *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;
}

int CircularQueueIsEmpty(const CircularQueue *queue)
{
    return queue->count == 0;
}

int CircularQueueIsFull(const CircularQueue *queue)
{
    return queue->count == CIRCULAR_QUEUE_CAPACITY;
}

int CircularQueueEnqueue(CircularQueue *queue, int value)
{
    if (CircularQueueIsFull(queue))
        return 0;

    queue->rear = (queue->rear + 1) % CIRCULAR_QUEUE_CAPACITY;
    queue->items[queue->rear] = value;
    queue->count++;
    return 1;
}

int CircularQueueDequeue(CircularQueue *queue, int *removedValue)
{
    if (CircularQueueIsEmpty(queue))
        return 0;

    *removedValue = queue->items[queue->front];
    queue->front = (queue->front + 1) % CIRCULAR_QUEUE_CAPACITY;
    queue->count--;

    if (queue->count == 0)
    {
        queue->front = 0;
        queue->rear = -1;
    }

    return 1;
}

int CircularQueuePeek(const CircularQueue *queue, int *frontValue)
{
    if (CircularQueueIsEmpty(queue))
        return 0;

    *frontValue = queue->items[queue->front];
    return 1;
}

int CircularQueueSize(const CircularQueue *queue)
{
    return queue->count;
}
