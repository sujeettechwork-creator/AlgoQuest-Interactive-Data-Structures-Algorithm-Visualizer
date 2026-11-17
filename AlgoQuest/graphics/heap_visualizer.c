#include "heap_visualizer.h"
#include "ui.h"
#include "../structures/heap.h"

#include <math.h>
#include <stdio.h>

#define HEAP_UI_MAX 31
#define ROW_CELLS 16

static Heap heap;
static char buf[8];
static int active = 1;
static UIStatus st;
static float px[HEAP_UI_MAX], py[HEAP_UI_MAX], flash[HEAP_UI_MAX];
static int seen[HEAP_UI_MAX];
static int prevVal[HEAP_UI_MAX];
static int prevSize = 0;

void HeapVisualizerInit(void)
{
    HeapInit(&heap, 0);
    buf[0] = '\0';
    active = 1;
    prevSize = 0;

    for (int i = 0; i < HEAP_UI_MAX; i++)
    {
        px[i] = py[i] = flash[i] = 0.0f;
        seen[i] = 0;
        prevVal[i] = 0;
    }

    UISetStatus(&st, "Max heap ready. Insert values and watch them bubble up.", C_AMBER);
}

static void DoInsert(void)
{
    int v;

    if (!UIParseInt(buf, &v))
    {
        UISetStatus(&st, "Enter a valid integer.", C_RED);
        return;
    }

    if (heap.size >= HEAP_UI_MAX)
    {
        UISetStatus(&st, "Display limit reached (31 items).", C_AMBER);
        return;
    }

    if (HeapInsert(&heap, v))
    {
        UISetStatus(&st, TextFormat("Inserted %d at the end, then sifted up (heapify-up).", v), C_GREEN);
        buf[0] = '\0';
    }
    else
        UISetStatus(&st, "Heap is full.", C_RED);
}

void HeapVisualizerDraw(void)
{
    UIBackground();
    UIHeader("HEAP & PRIORITY QUEUE", "Complete binary tree stored in an array   |   parent beats its children", C_AMBER);

    UICard((Rectangle){30, 100, 890, 140}, "OPERATIONS", C_AMBER);

    if (UIInput((Rectangle){50, 135, 170, 46}, buf, sizeof(buf), &active, "Enter value", 0))
        DoInsert();

    if (UIButton((Rectangle){235, 135, 115, 46}, "INSERT", C_GREEN))
        DoInsert();

    if (UIButton((Rectangle){360, 135, 140, 46}, "EXTRACT ROOT", C_PURPLE))
    {
        int r;

        if (HeapExtractRoot(&heap, &r))
            UISetStatus(&st, TextFormat("Extracted root %d; last item moved to the top and sifted down.", r), C_GREEN);
        else
            UISetStatus(&st, "Heap is empty.", C_RED);
    }

    if (UIButton((Rectangle){510, 135, 95, 46}, "PEEK", C_BLUE))
    {
        int r;

        if (HeapPeek(&heap, &r))
            UISetStatus(&st, TextFormat("Root (highest priority) is %d.", r), C_CYAN);
        else
            UISetStatus(&st, "Heap is empty.", C_RED);
    }

    if (UIButton((Rectangle){615, 135, 125, 46}, "ADD 5 RANDOM", C_CYAN))
    {
        int added = 0;

        for (int i = 0; i < 5 && heap.size < HEAP_UI_MAX; i++)
            added += HeapInsert(&heap, GetRandomValue(1, 99));

        UISetStatus(&st, TextFormat("Inserted %d random value(s).", added), C_GREEN);
    }

    if (UIButton((Rectangle){750, 135, 100, 46}, "CLEAR", C_RED))
    {
        HeapClear(&heap);
        UISetStatus(&st, "Heap cleared.", C_AMBER);
    }

    if (UIButton((Rectangle){50, 192, 210, 36}, heap.isMinHeap ? "MODE: MIN HEAP  (toggle)" : "MODE: MAX HEAP  (toggle)", C_PANEL2))
    {
        int vals[HEAP_UI_MAX];
        int n = heap.size < HEAP_UI_MAX ? heap.size : HEAP_UI_MAX;

        for (int i = 0; i < n; i++)
            vals[i] = heap.data[i];

        HeapInit(&heap, !heap.isMinHeap);

        for (int i = 0; i < n; i++)
            HeapInsert(&heap, vals[i]);

        UISetStatus(&st, heap.isMinHeap ? "Switched to MIN heap: smallest value is the root."
                                        : "Switched to MAX heap: largest value is the root.", C_AMBER);
    }

    DrawText("parent(i) = (i-1)/2     left(i) = 2i+1     right(i) = 2i+2", 280, 202, 14, C_MUTED);
    DrawText(TextFormat("SIZE  %d / %d", heap.size, HEAP_UI_MAX), 760, 202, 14, C_AMBER);

    /* detect changes so moved values flash */
    for (int i = 0; i < HEAP_UI_MAX; i++)
    {
        if (i < heap.size)
        {
            if (i >= prevSize || heap.data[i] != prevVal[i])
                flash[i] = 1.0f;
            prevVal[i] = heap.data[i];
        }
        else
        {
            seen[i] = 0;
            flash[i] = 0.0f;
        }
    }
    prevSize = heap.size;

    /* array cell rectangles + hover detection */
    Vector2 mouse = GetMousePosition();
    int hover = -1;

    for (int i = 0; i < heap.size; i++)
    {
        Rectangle cell = {38.0f + (i % ROW_CELLS) * 55.0f, 592.0f + (i / ROW_CELLS) * 50.0f, 50, 30};
        if (CheckCollisionPointRec(mouse, cell))
            hover = i;
    }

    Rectangle canvas = {30, 250, 890, 310};
    UICard(canvas, "HEAP TREE", C_AMBER);

    float dy = 58.0f;

    for (int i = 0; i < heap.size; i++)
    {
        int depth = 0;
        while ((1 << (depth + 1)) - 1 <= i)
            depth++;

        int first = (1 << depth) - 1;
        int slots = 1 << depth;
        float tx = canvas.x + ((float)(i - first) + 0.5f) / (float)slots * canvas.width;
        float ty = canvas.y + 52.0f + depth * dy;

        if (!seen[i])
        {
            int p = i > 0 ? (i - 1) / 2 : 0;
            px[i] = i > 0 ? px[p] : tx;
            py[i] = i > 0 ? py[p] : ty;
            seen[i] = 1;
        }

        px[i] = UIApproach(px[i], tx, 9.0f);
        py[i] = UIApproach(py[i], ty, 9.0f);
        flash[i] = UIApproach(flash[i], 0.0f, 1.6f);
    }

    if (heap.size == 0)
        DrawText("Heap is empty  -  insert values to build the tree.", 300, 400, 20, Fade(C_MUTED, 0.8f));

    for (int i = 1; i < heap.size; i++)
    {
        int p = (i - 1) / 2;
        int lit = hover == i || hover == p;
        DrawLineEx((Vector2){px[p], py[p]}, (Vector2){px[i], py[i]}, lit ? 3.5f : 2.0f,
                   lit ? C_AMBER : (Color){80, 96, 140, 255});
    }

    for (int i = 0; i < heap.size; i++)
    {
        Color fill = i == 0 ? C_AMBER : C_PANEL2;
        Color border = i == 0 ? C_AMBER : C_CYAN;
        float glow = flash[i];

        if (hover >= 0)
        {
            if (i == hover) { border = C_WHITE; glow = 1.0f; }
            else if (hover > 0 && i == (hover - 1) / 2) { border = C_PURPLE; glow = 1.0f; }
            else if (i == 2 * hover + 1 || i == 2 * hover + 2) { border = C_GREEN; glow = 1.0f; }
        }

        if (flash[i] > 0.05f)
            border = UILerpColor(border, C_AMBER, flash[i]);

        UINode((Vector2){px[i], py[i]}, 20.0f, TextFormat("%d", heap.data[i]), fill, border, glow);
        DrawText(TextFormat("%d", i), (int)px[i] + 18, (int)py[i] - 28, 11, C_MUTED);
    }

    DrawText("ARRAY REPRESENTATION   (hover a cell: white = it, purple = parent, green = children)", 32, 567, 13, C_AMBER);

    for (int i = 0; i < heap.size; i++)
    {
        Rectangle cell = {38.0f + (i % ROW_CELLS) * 55.0f, 592.0f + (i / ROW_CELLS) * 50.0f, 50, 30};
        Color fill = UILerpColor(i == 0 ? (Color){110, 80, 20, 255} : C_PANEL2, (Color){130, 90, 20, 255}, flash[i]);

        UICell(cell, TextFormat("%d", heap.data[i]), fill, hover == i ? C_WHITE : C_CYAN, 15);
        DrawText(TextFormat("%d", i), (int)cell.x + 22 - (i >= 10 ? 3 : 0), (int)cell.y + 33, 11, C_MUTED);
    }

    UIStatusBar(&st, (Rectangle){30, 690, 890, 52});
}
