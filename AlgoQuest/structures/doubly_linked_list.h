#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

typedef struct DNode
{
    int data;
    struct DNode *prev;
    struct DNode *next;
} DNode;

typedef struct
{
    DNode *head;
    DNode *tail;
    int size;
} DoublyLinkedList;

void DLLInit(DoublyLinkedList *list);
int DLLInsertFront(DoublyLinkedList *list, int value);
int DLLInsertEnd(DoublyLinkedList *list, int value);
int DLLInsertAt(DoublyLinkedList *list, int position, int value);
int DLLDeleteValue(DoublyLinkedList *list, int value);
int DLLSearch(const DoublyLinkedList *list, int value);
void DLLReverse(DoublyLinkedList *list);
void DLLClear(DoublyLinkedList *list);

#endif