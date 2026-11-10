
/* structures/heap.h */
#ifndef HEAP_H
#define HEAP_H

#define HEAP_CAPACITY 100

typedef struct {
    int data[HEAP_CAPACITY];
    int size;
    int isMinHeap;
} Heap;

void HeapInit(Heap *heap, int isMinHeap);
int HeapInsert(Heap *heap, int value);
int HeapExtractRoot(Heap *heap, int *result);
int HeapPeek(const Heap *heap, int *result);
void HeapClear(Heap *heap);
int HeapIsEmpty(const Heap *heap);
int HeapIsFull(const Heap *heap);

#endif
