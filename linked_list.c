#include "linked_list.h"
#include <stdlib.h>

void LinkedListInit(LinkedList *list)
{
    list->head = NULL;
    list->size = 0;
}

int LinkedListInsertFront(LinkedList *list, int value)
{
    Node *newNode = malloc(sizeof(Node));

    if (newNode == NULL)
        return 0;

    newNode->data = value;
    newNode->next = list->head;

    list->head = newNode;
    list->size++;

    return 1;
}

int LinkedListInsertEnd(LinkedList *list, int value)
{
    Node *newNode = malloc(sizeof(Node));

    if (newNode == NULL)
        return 0;

    newNode->data = value;
    newNode->next = NULL;

    if (list->head == NULL)
        list->head = newNode;
    else
    {
        Node *temp = list->head;

        while (temp->next != NULL)
            temp = temp->next;

        temp->next = newNode;
    }

    list->size++;

    return 1;
}

int LinkedListInsertAt(LinkedList *list, int index, int value)
{
    if (index < 0 || index > list->size)
        return 0;
    if (index == 0)
        return LinkedListInsertFront(list, value);
    if (index == list->size)
        return LinkedListInsertEnd(list, value);

    Node *newNode = malloc(sizeof(Node));
    if (newNode == NULL)
        return 0;

    newNode->data = value;

    Node *temp = list->head;
    for (int i = 0; i < index - 1; i++)
    {
        temp = temp->next;
    }

    newNode->next = temp->next;
    temp->next = newNode;
    list->size++;

    return 1;
}

int LinkedListDeleteValue(LinkedList *list, int value)
{
    Node *temp = list->head;
    Node *previous = NULL;

    while (temp != NULL && temp->data != value)
    {
        previous = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return 0;

    if (previous == NULL)
        list->head = temp->next;
    else
        previous->next = temp->next;

    free(temp);
    list->size--;

    return 1;
}

int LinkedListSearch(const LinkedList *list, int value)
{
    Node *temp = list->head;
    int position = 0;

    while (temp != NULL)
    {
        if (temp->data == value)
            return position;

        temp = temp->next;
        position++;
    }

    return -1;
}

void LinkedListClear(LinkedList *list)
{
    Node *temp = list->head;

    while (temp != NULL)
    {
        Node *next = temp->next;
        free(temp);
        temp = next;
    }

    list->head = NULL;
    list->size = 0;
}