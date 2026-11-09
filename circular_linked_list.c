#include "circular_linked_list.h"
#include <stdlib.h>

void CLLInit(CircularLinkedList *list)
{
    list->tail = NULL;
    list->size = 0;
}

int CLLInsertFront(CircularLinkedList *list, int value)
{
    CNode *node = malloc(sizeof(CNode));

    if (node == NULL)
        return 0;

    if (list->tail == NULL)
    {
        node->next = node;
        list->tail = node;
    }
    else
    {
        node->next = list->tail->next;
        list->tail->next = node;
    }

    node->data = value;
    list->size++;

    return 1;
}

int CLLInsertEnd(CircularLinkedList *list, int value)
{
    if (!CLLInsertFront(list, value))
        return 0;

    list->tail = list->tail->next;

    return 1;
}

int CLLInsertAt(CircularLinkedList *list, int position, int value)
{
    if (position < 0 || position > list->size)
        return 0;

    if (position == 0)
        return CLLInsertFront(list, value);

    if (position == list->size)
        return CLLInsertEnd(list, value);

    CNode *previous = list->tail->next;

    for (int i = 0; i < position - 1; i++)
        previous = previous->next;

    CNode *node = malloc(sizeof(CNode));

    if (node == NULL)
        return 0;

    node->data = value;
    node->next = previous->next;
    previous->next = node;
    list->size++;

    return 1;
}

int CLLDeleteValue(CircularLinkedList *list, int value)
{
    if (list->tail == NULL)
        return 0;

    CNode *current = list->tail->next;
    CNode *previous = list->tail;

    do
    {
        if (current->data == value)
        {
            if (current == list->tail)
                list->tail = previous;

            if (current == previous)
            {
                list->tail = NULL;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            list->size--;

            return 1;
        }

        previous = current;
        current = current->next;

    } while (current != list->tail->next);

    return 0;
}

int CLLSearch(const CircularLinkedList *list, int value)
{
    if (list->tail == NULL)
        return -1;

    CNode *current = list->tail->next;
    int position = 0;

    do
    {
        if (current->data == value)
            return position;

        current = current->next;
        position++;

    } while (current != list->tail->next);

    return -1;
}

void CLLReverse(CircularLinkedList *list)
{
    if (list->size <= 1)
        return;

    CNode *head = list->tail->next;
    CNode *previous = list->tail;
    CNode *current = head;

    do
    {
        CNode *next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    } while (current != head);

    list->tail = head;
}

void CLLClear(CircularLinkedList *list)
{
    if (list->tail == NULL)
        return;

    CNode *head = list->tail->next;
    CNode *current = head;

    do
    {
        CNode *next = current->next;
        free(current);
        current = next;
    } while (current != head);

    list->tail = NULL;
    list->size = 0;
}