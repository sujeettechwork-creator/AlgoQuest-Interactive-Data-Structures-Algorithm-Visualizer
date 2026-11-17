#include "list_view.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NODE_W 94.0f
#define NODE_H 64.0f
#define PITCH 138.0f
#define X0 60.0f
#define Y0 400.0f
#define VISIBLE 6
#define MAX_ITEMS 30

static int Size(const ListView *v)
{
    switch (v->kind)
    {
    case LV_SINGLY: return v->sll.size;
    case LV_DOUBLY: return v->dll.size;
    default: return v->cll.size;
    }
}

static int ValueAt(const ListView *v, int idx)
{
    if (v->kind == LV_SINGLY)
    {
        const Node *n = v->sll.head;
        while (n != NULL && idx-- > 0)
            n = n->next;
        return n ? n->data : 0;
    }

    if (v->kind == LV_DOUBLY)
    {
        const DNode *n = v->dll.head;
        while (n != NULL && idx-- > 0)
            n = n->next;
        return n ? n->data : 0;
    }

    const CNode *n = v->cll.tail ? v->cll.tail->next : NULL;
    while (n != NULL && idx-- > 0)
        n = n->next;
    return n ? n->data : 0;
}

static int InsertAt(ListView *v, int pos, int value)
{
    switch (v->kind)
    {
    case LV_SINGLY: return LinkedListInsertAt(&v->sll, pos, value);
    case LV_DOUBLY: return DLLInsertAt(&v->dll, pos, value);
    default: return CLLInsertAt(&v->cll, pos, value);
    }
}

static int DeleteValue(ListView *v, int value)
{
    switch (v->kind)
    {
    case LV_SINGLY: return LinkedListDeleteValue(&v->sll, value);
    case LV_DOUBLY: return DLLDeleteValue(&v->dll, value);
    default: return CLLDeleteValue(&v->cll, value);
    }
}

static int Search(const ListView *v, int value)
{
    switch (v->kind)
    {
    case LV_SINGLY: return LinkedListSearch(&v->sll, value);
    case LV_DOUBLY: return DLLSearch(&v->dll, value);
    default: return CLLSearch(&v->cll, value);
    }
}

static void Reverse(ListView *v)
{
    switch (v->kind)
    {
    case LV_SINGLY: LinkedListReverse(&v->sll); break;
    case LV_DOUBLY: DLLReverse(&v->dll); break;
    default: CLLReverse(&v->cll); break;
    }
}

static void ClearAll(ListView *v)
{
    switch (v->kind)
    {
    case LV_SINGLY: LinkedListClear(&v->sll); break;
    case LV_DOUBLY: DLLClear(&v->dll); break;
    default: CLLClear(&v->cll); break;
    }
}

static void SetScroll(ListView *v, int s)
{
    int max = Size(v) - VISIBLE;

    if (max < 0) max = 0;
    if (s < 0) s = 0;
    if (s > max) s = max;

    v->slide += (float)(s - v->scroll);
    v->scroll = s;
}

static void Reveal(ListView *v, int index)
{
    if (index < v->scroll)
        SetScroll(v, index);
    else if (index >= v->scroll + VISIBLE)
        SetScroll(v, index - VISIBLE + 1);
}

void ListViewInit(ListView *v, ListKind kind)
{
    ClearAll(v);
    v->kind = kind;

    LinkedListInit(&v->sll);
    DLLInit(&v->dll);
    CLLInit(&v->cll);

    v->buf[0] = '\0';
    v->idxBuf[0] = '\0';
    v->active = 1;
    v->idxActive = 0;
    v->scroll = 0;
    v->slide = 0.0f;
    v->hl = -1;
    v->hlT = 0.0f;

    UISetStatus(&v->st, "Enter a value and choose an operation.", C_CYAN);
}

static void DoInsert(ListView *v, int mode)
{
    int value, pos;
    char msg[140];

    if (!UIParseInt(v->buf, &value))
    {
        UISetStatus(&v->st, "Enter a valid integer value first.", C_RED);
        return;
    }

    int size = Size(v);

    if (size >= MAX_ITEMS)
    {
        UISetStatus(&v->st, "Display limit reached (30 nodes). Delete or clear first.", C_AMBER);
        return;
    }

    if (mode == 0)
        pos = 0;
    else if (mode == 1)
        pos = size;
    else
    {
        if (!UIParseInt(v->idxBuf, &pos))
        {
            UISetStatus(&v->st, "Enter an index in the small Index box.", C_RED);
            return;
        }
        if (pos < 0 || pos > size)
        {
            snprintf(msg, sizeof(msg), "Index out of range: valid indices are 0 to %d.", size);
            UISetStatus(&v->st, msg, C_RED);
            return;
        }
    }

    if (InsertAt(v, pos, value))
    {
        v->hl = pos;
        v->hlT = 1.0f;
        Reveal(v, pos);
        snprintf(msg, sizeof(msg), "Inserted %d at index %d.", value, pos);
        UISetStatus(&v->st, msg, C_GREEN);
        v->buf[0] = '\0';
    }
    else
        UISetStatus(&v->st, "Memory allocation failed.", C_RED);
}

static void DrawListNode(const ListView *v, int i, int size)
{
    float x = X0 + (i - v->scroll + v->slide) * PITCH;
    float a = 1.0f;

    if (x < 40.0f)
        a = (x - (40.0f - PITCH)) / PITCH;
    else if (x > CW - 40.0f - NODE_W)
        a = ((CW - 40.0f) - x) / PITCH;

    a = UIClampf(a, 0.0f, 1.0f);
    if (a <= 0.01f)
        return;

    float pulse = (i == v->hl) ? v->hlT : 0.0f;
    float pop = 1.0f - 0.25f * pulse;
    float midY = Y0 + NODE_H / 2;
    Rectangle r = {x + NODE_W * (1 - pop) / 2, Y0 + NODE_H * (1 - pop) / 2, NODE_W * pop, NODE_H * pop};
    Color fill = (i == 0) ? (Color){52, 44, 105, 255} : C_PANEL2;
    Color border = UILerpColor(C_CYAN, C_AMBER, pulse);

    if (pulse > 0.02f)
        DrawRectangleRounded((Rectangle){r.x - 6, r.y - 6, r.width + 12, r.height + 12},
                             0.3f, 8, Fade(C_AMBER, 0.25f * pulse * a));

    DrawRectangleRounded(r, 0.18f, 8, Fade(fill, a));
    DrawRectangleRoundedLinesEx(r, 0.18f, 8, 2.0f, Fade(border, a));

    const char *value = TextFormat("%d", ValueAt(v, i));

    if (v->kind == LV_DOUBLY)
    {
        DrawLineEx((Vector2){r.x + r.width * 0.24f, r.y}, (Vector2){r.x + r.width * 0.24f, r.y + r.height}, 1.5f, Fade(border, 0.5f * a));
        DrawLineEx((Vector2){r.x + r.width * 0.76f, r.y}, (Vector2){r.x + r.width * 0.76f, r.y + r.height}, 1.5f, Fade(border, 0.5f * a));
        DrawCircleV((Vector2){r.x + r.width * 0.12f, midY}, 4.5f, Fade(C_CYAN, a));
        DrawCircleV((Vector2){r.x + r.width * 0.88f, midY}, 4.5f, Fade(C_GREEN, a));
        DrawText(value, (int)(r.x + r.width * 0.5f - MeasureText(value, 20) / 2.0f), (int)midY - 10, 20, Fade(C_WHITE, a));
    }
    else
    {
        DrawLineEx((Vector2){r.x + r.width * 0.68f, r.y}, (Vector2){r.x + r.width * 0.68f, r.y + r.height}, 1.5f, Fade(border, 0.5f * a));
        DrawCircleV((Vector2){r.x + r.width * 0.84f, midY}, 4.5f, Fade(C_GREEN, a));
        DrawText(value, (int)(r.x + r.width * 0.34f - MeasureText(value, 20) / 2.0f), (int)midY - 10, 20, Fade(C_WHITE, a));
    }

    if (i == 0 && i == size - 1)
        DrawText("HEAD / TAIL", (int)x, (int)Y0 - 24, 13, Fade(C_PURPLE, a));
    else if (i == 0)
        DrawText("HEAD", (int)x, (int)Y0 - 24, 13, Fade(C_PURPLE, a));
    else if (i == size - 1)
        DrawText("TAIL", (int)x, (int)Y0 - 24, 13, Fade(C_GREEN, a));

    const char *idx = TextFormat("[%d]", i);
    DrawText(idx, (int)(x + NODE_W / 2 - MeasureText(idx, 13) / 2.0f), (int)(Y0 + NODE_H + 8), 13, Fade(C_MUTED, a));

    int hasNext = i < size - 1;
    float nx = x + PITCH;

    if (v->kind == LV_DOUBLY)
    {
        if (hasNext)
        {
            UIArrow((Vector2){x + NODE_W, Y0 + NODE_H * 0.33f}, (Vector2){nx - 2, Y0 + NODE_H * 0.33f}, Fade(C_GREEN, a), 2.5f);
            UIArrow((Vector2){nx, Y0 + NODE_H * 0.67f}, (Vector2){x + NODE_W + 2, Y0 + NODE_H * 0.67f}, Fade(C_CYAN, a), 2.5f);
        }
        else
        {
            UIArrow((Vector2){x + NODE_W, Y0 + NODE_H * 0.33f}, (Vector2){x + NODE_W + 34, Y0 + NODE_H * 0.33f}, Fade(C_GREEN, a), 2.5f);
            DrawText("NULL", (int)(x + NODE_W + 38), (int)(Y0 + NODE_H * 0.33f) - 7, 14, Fade(C_RED, a));
        }

        if (i == 0)
        {
            UIArrow((Vector2){x, Y0 + NODE_H * 0.67f}, (Vector2){x - 30, Y0 + NODE_H * 0.67f}, Fade(C_CYAN, a), 2.5f);
            DrawText("NULL", (int)(x - 62), (int)(Y0 + NODE_H * 0.67f) - 7, 14, Fade(C_RED, a));
        }
        return;
    }

    float dotX = x + NODE_W * 0.84f;

    if (hasNext)
        UIArrow((Vector2){dotX, midY}, (Vector2){nx - 2, midY}, Fade(C_GREEN, a), 2.5f);
    else if (v->kind == LV_SINGLY)
    {
        UIArrow((Vector2){dotX, midY}, (Vector2){x + NODE_W + 34, midY}, Fade(C_GREEN, a), 2.5f);
        DrawText("NULL", (int)(x + NODE_W + 38), (int)midY - 7, 14, Fade(C_RED, a));
    }
    else
    {
        /* circular: tail points back to head */
        float by = Y0 + NODE_H + 52;
        float headX = X0 + (0 - v->scroll + v->slide) * PITCH + NODE_W * 0.5f;
        int headVisible = (v->scroll == 0);
        float tx = headVisible ? headX : X0 - 30;
        Color c = Fade(C_PURPLE, a);

        DrawLineEx((Vector2){dotX, midY + 6}, (Vector2){dotX, by}, 2.5f, c);
        DrawLineEx((Vector2){dotX, by}, (Vector2){tx, by}, 2.5f, c);
        UIArrow((Vector2){tx, by}, (Vector2){tx, Y0 + NODE_H + 4}, c, 2.5f);

        const char *label = headVisible ? "TAIL -> HEAD  (circular link)" : "TAIL -> HEAD  (head is off-screen)";
        DrawText(label, (int)((dotX + tx) / 2 - MeasureText(label, 13) / 2.0f), (int)by + 8, 13, c);
    }
}

void ListViewDraw(ListView *v)
{
    static const char *titles[] = {"LINKED LIST VISUALIZER", "DOUBLY LINKED LIST VISUALIZER", "CIRCULAR LINKED LIST VISUALIZER"};
    static const char *subs[] = {
        "Singly linked  -  each node stores data and a next pointer",
        "Doubly linked  -  each node stores prev and next pointers",
        "Circular  -  the tail node points back to the head"};
    static const char *costs[] = {
        "Insert / delete at HEAD: O(1)     Insert at TAIL: O(n)     Search: O(n)     Reverse: O(n)",
        "Insert / delete at HEAD or TAIL: O(1)     Search: O(n)     Reverse: O(n)     Extra memory: one pointer per node",
        "Insert at HEAD or TAIL: O(1) with a tail pointer     Search: O(n)     Reverse: O(n)     No NULL at the end"};

    Color accent = v->kind == LV_SINGLY ? C_CYAN : (v->kind == LV_DOUBLY ? C_PURPLE : C_GREEN);

    SetScroll(v, v->scroll);
    v->slide = UIApproach(v->slide, 0.0f, 10.0f);
    v->hlT = UIApproach(v->hlT, 0.0f, 1.6f);

    UIBackground();
    UIHeader(titles[v->kind], subs[v->kind], accent);

    UICard((Rectangle){30, 100, 890, 165}, "OPERATIONS", accent);

    if (UIInput((Rectangle){50, 135, 150, 46}, v->buf, sizeof(v->buf), &v->active, "Value", 0))
        DoInsert(v, 1);
    UIInput((Rectangle){210, 135, 110, 46}, v->idxBuf, sizeof(v->idxBuf), &v->idxActive, "Index", 0);

    if (UIButton((Rectangle){340, 135, 140, 46}, "INSERT FRONT", C_CYAN))
        DoInsert(v, 0);
    if (UIButton((Rectangle){490, 135, 140, 46}, "INSERT END", C_BLUE))
        DoInsert(v, 1);
    if (UIButton((Rectangle){640, 135, 160, 46}, "INSERT AT INDEX", C_PURPLE))
        DoInsert(v, 2);

    if (UIButton((Rectangle){50, 197, 150, 46}, "DELETE VALUE", C_RED))
    {
        int value;
        char msg[120];

        if (!UIParseInt(v->buf, &value))
            UISetStatus(&v->st, "Enter the value to delete.", C_RED);
        else
        {
            int pos = Search(v, value);

            if (DeleteValue(v, value))
            {
                snprintf(msg, sizeof(msg), "Deleted the first node holding %d (was at index %d).", value, pos);
                UISetStatus(&v->st, msg, C_GREEN);
                v->hl = -1;
                v->buf[0] = '\0';
            }
            else
                UISetStatus(&v->st, "Value not found.", C_RED);
        }
    }

    if (UIButton((Rectangle){210, 197, 120, 46}, "SEARCH", C_BLUE))
    {
        int value;
        char msg[120];

        if (!UIParseInt(v->buf, &value))
            UISetStatus(&v->st, "Enter the value to search for.", C_RED);
        else
        {
            int pos = Search(v, value);

            if (pos >= 0)
            {
                v->hl = pos;
                v->hlT = 1.0f;
                Reveal(v, pos);
                snprintf(msg, sizeof(msg), "Found %d at index %d after %d comparison(s).", value, pos, pos + 1);
                UISetStatus(&v->st, msg, C_GREEN);
            }
            else
            {
                snprintf(msg, sizeof(msg), "%d not found after %d comparison(s).", value, Size(v));
                UISetStatus(&v->st, msg, C_RED);
            }
        }
    }

    if (UIButton((Rectangle){340, 197, 120, 46}, "REVERSE", C_PURPLE))
    {
        Reverse(v);
        SetScroll(v, 0);
        v->hl = -1;
        UISetStatus(&v->st, "List reversed by re-pointing every link.", C_GREEN);
    }

    if (UIButton((Rectangle){470, 197, 150, 46}, "ADD 5 RANDOM", C_GREEN))
    {
        int added = 0;

        for (int i = 0; i < 5 && Size(v) < MAX_ITEMS; i++)
            added += InsertAt(v, Size(v), GetRandomValue(1, 99));

        Reveal(v, Size(v) - 1);
        UISetStatus(&v->st, TextFormat("Appended %d random node(s).", added), C_GREEN);
    }

    if (UIButton((Rectangle){630, 197, 120, 46}, "CLEAR", C_RED))
    {
        ClearAll(v);
        v->scroll = 0;
        v->slide = 0.0f;
        v->hl = -1;
        UISetStatus(&v->st, "All nodes freed.", C_AMBER);
    }

    int size = Size(v);

    UICard((Rectangle){30, 280, 890, 265}, "MEMORY LAYOUT", accent);
    DrawText(TextFormat("SIZE  %d NODES", size), 770, 293, 14, accent);

    if (UIButton((Rectangle){30 + 890 - 170, 330, 70, 30}, "< PREV", C_PANEL2))
        SetScroll(v, v->scroll - 1);
    if (UIButton((Rectangle){30 + 890 - 90, 330, 70, 30}, "NEXT >", C_PANEL2))
        SetScroll(v, v->scroll + 1);

    float wheel = GetMouseWheelMove();
    if (wheel > 0.0f || IsKeyPressed(KEY_LEFT))
        SetScroll(v, v->scroll - 1);
    if (wheel < 0.0f || IsKeyPressed(KEY_RIGHT))
        SetScroll(v, v->scroll + 1);

    if (size == 0)
    {
        const char *t = v->kind == LV_SINGLY ? "HEAD -> NULL   (empty list)" : "EMPTY LIST";
        DrawText(t, (int)(CW / 2 - MeasureText(t, 24) / 2.0f), (int)Y0 + 20, 24, Fade(C_MUTED, 0.8f));
    }
    else
    {
        for (int i = v->scroll - 1; i <= v->scroll + VISIBLE; i++)
            if (i >= 0 && i < size)
                DrawListNode(v, i, size);

        if (size > VISIBLE)
            DrawText(TextFormat("Showing nodes %d-%d of %d   (mouse wheel, arrow keys or PREV / NEXT)",
                                v->scroll, v->scroll + VISIBLE - 1, size),
                     50, 520, 13, C_MUTED);
    }

    UIStatusBar(&v->st, (Rectangle){30, 560, 890, 76});

    UICard((Rectangle){30, 650, 890, 80}, "COMPLEXITY", accent);
    DrawText(costs[v->kind], 56, 685, 14, C_WHITE);
}
