#ifndef LIST_VIEW_H
#define LIST_VIEW_H

#include "ui.h"
#include "../structures/linked_list.h"
#include "../structures/doubly_linked_list.h"
#include "../structures/circular_linked_list.h"

typedef enum
{
    LV_SINGLY,
    LV_DOUBLY,
    LV_CIRCULAR
} ListKind;

typedef struct
{
    ListKind kind;
    LinkedList sll;
    DoublyLinkedList dll;
    CircularLinkedList cll;

    char buf[8];
    char idxBuf[8];
    int active;
    int idxActive;
    UIStatus st;

    int scroll;
    float slide;
    int hl;
    float hlT;
} ListView;

void ListViewInit(ListView *v, ListKind kind);
void ListViewDraw(ListView *v);

#endif
