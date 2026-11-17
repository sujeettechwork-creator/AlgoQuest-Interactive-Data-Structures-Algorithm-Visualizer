#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include "ui.h"

#define TV_MAX 64

/* Flat copy of a tree. left/right are indices into the array (-1 = none).
   Nodes must be stored parent-before-child (preorder). */
typedef struct
{
    int value;
    int left;
    int right;
    int balance;
    float tx, ty;
} TVNode;

typedef struct
{
    int used[TV_MAX];
    int value[TV_MAX];
    float x[TV_MAX];
    float y[TV_MAX];

    int order[TV_MAX];
    int orderCount;
    float progress;
    int found; /* -1 = plain traversal, 0 = search failed, 1 = search succeeded */
} TVState;

void TVReset(TVState *s);
void TVSetOrder(TVState *s, const int *order, int n, int found);
void TVDraw(TVState *s, TVNode *nodes, int count, Rectangle canvas, int showBalance);

#endif
