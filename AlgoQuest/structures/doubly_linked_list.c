#include "doubly_linked_list.h"
#include <stdlib.h>

void DLLInit(DoublyLinkedList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

int DLLInsertFront(DoublyLinkedList *list, int value)
{
    DNode *node = malloc(sizeof(DNode));

    if (node == NULL)
        return 0;

    node->data = value;
    node->prev = NULL;
    node->next = list->head;

    if (list->head != NULL)
        list->head->prev = node;
    else
        list->tail = node;

    list->head = node;
    list->size++;

    return 1;
}

int DLLInsertEnd(DoublyLinkedList *list, int value)
{
    DNode *node = malloc(sizeof(DNode));

    if (node == NULL)
        return 0;

    node->data = value;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail != NULL)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;
    list->size++;

    return 1;
}

int DLLInsertAt(DoublyLinkedList *list, int position, int value)
{
    if (position < 0 || position > list->size)
        return 0;

    if (position == 0)
        return DLLInsertFront(list, value);

    if (position == list->size)
        return DLLInsertEnd(list, value);

    DNode *current = list->head;

    for (int i = 0; i < position; i++)
        current = current->next;

    DNode *node = malloc(sizeof(DNode));

    if (node == NULL)
        return 0;

    node->data = value;
    node->prev = current->prev;
    node->next = current;

    current->prev->next = node;
    current->prev = node;

    list->size++;

    return 1;
}

int DLLDeleteValue(DoublyLinkedList *list, int value)
{
    DNode *current = list->head;

    while (current != NULL && current->data != value)
        current = current->next;

    if (current == NULL)
        return 0;

    if (current->prev != NULL)
        current->prev->next = current->next;
    else
        list->head = current->next;

    if (current->next != NULL)
        current->next->prev = current->prev;
    else
        list->tail = current->prev;

    free(current);
    list->size--;

    return 1;
}

int DLLSearch(const DoublyLinkedList *list, int value)
{
    DNode *current = list->head;
    int position = 0;

    while (current != NULL)
    {
        if (current->data == value)
            return position;

        current = current->next;
        position++;
    }

    return -1;
}

void DLLReverse(DoublyLinkedList *list)
{
    DNode *current = list->head;
    DNode *temp = NULL;

    while (current != NULL)
    {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;

        current = current->prev;
    }

    temp = list->head;
    list->head = list->tail;
    list->tail = temp;
}

void DLLClear(DoublyLinkedList *list)
{
    DNode *current = list->head;

    while (current != NULL)
    {
        DNode *next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}