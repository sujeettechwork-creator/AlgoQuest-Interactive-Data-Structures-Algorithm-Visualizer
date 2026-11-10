
/* structures/heap.c */
#include "heap.h"

static int HasHigherPriority(const Heap *heap, int a, int b)
{
    if (heap->isMinHeap)
        return a < b;

    return a > b;
}

static void Swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void HeapifyUp(Heap *heap, int index)
{
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (!HasHigherPriority(heap, heap->data[index],
                               heap->data[parent]))
            break;

        Swap(&heap->data[index], &heap->data[parent]);
        index = parent;
    }
}

static void HeapifyDown(Heap *heap, int index)
{
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int priority = index;

        if (left < heap->size &&
            HasHigherPriority(heap, heap->data[left],
                              heap->data[priority]))
            priority = left;

        if (right < heap->size &&
            HasHigherPriority(heap, heap->data[right],
                              heap->data[priority]))
            priority = right;

        if (priority == index)
            break;

        Swap(&heap->data[index], &heap->data[priority]);
        index = priority;
    }
}

void HeapInit(Heap *heap, int isMinHeap)
{
    heap->size = 0;
    heap->isMinHeap = isMinHeap ? 1 : 0;
}

int HeapInsert(Heap *heap, int value)
{
    if (HeapIsFull(heap))
        return 0;

    heap->data[heap->size] = value;
    heap->size++;

    HeapifyUp(heap, heap->size - 1);
    return 1;
}

int HeapExtractRoot(Heap *heap, int *result)
{
    if (HeapIsEmpty(heap))
        return 0;

    *result = heap->data[0];

    heap->size--;

    if (heap->size > 0) {
        heap->data[0] = heap->data[heap->size];
        HeapifyDown(heap, 0);
    }

    return 1;
}

int HeapPeek(const Heap *heap, int *result)
{
    if (HeapIsEmpty(heap))
        return 0;

    *result = heap->data[0];
    return 1;
}

void HeapClear(Heap *heap)
{
    heap->size = 0;
}

int HeapIsEmpty(const Heap *heap)
{
    return heap->size == 0;
}

int HeapIsFull(const Heap *heap)
{
    return heap->size >= HEAP_CAPACITY;
}
