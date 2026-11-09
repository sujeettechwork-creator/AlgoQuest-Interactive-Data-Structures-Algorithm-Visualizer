#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Node
{
    int data;
    struct Node *next;
} Node;

typedef struct
{
    Node *head;
    int size;
} LinkedList;

void LinkedListInit(LinkedList *list);
int LinkedListInsertFront(LinkedList *list, int value);
int LinkedListInsertEnd(LinkedList *list, int value);
int LinkedListInsertAt(LinkedList *list, int index, int value);
int LinkedListDeleteValue(LinkedList *list, int value);
int LinkedListSearch(const LinkedList *list, int value);
void LinkedListClear(LinkedList *list);

#endif