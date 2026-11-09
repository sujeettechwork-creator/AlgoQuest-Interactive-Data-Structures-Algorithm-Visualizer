#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H

typedef struct CNode
{
    int data;
    struct CNode *next;
} CNode;

typedef struct
{
    CNode *tail;
    int size;
} CircularLinkedList;

void CLLInit(CircularLinkedList *list);
int CLLInsertFront(CircularLinkedList *list, int value);
int CLLInsertEnd(CircularLinkedList *list, int value);
int CLLInsertAt(CircularLinkedList *list, int position, int value);
int CLLDeleteValue(CircularLinkedList *list, int value);
int CLLSearch(const CircularLinkedList *list, int value);
void CLLReverse(CircularLinkedList *list);
void CLLClear(CircularLinkedList *list);

#endif