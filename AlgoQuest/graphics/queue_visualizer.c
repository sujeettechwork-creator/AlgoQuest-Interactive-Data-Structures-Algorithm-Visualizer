#include "queue_visualizer.h"
#include "ui.h"
#include "../structures/queue.h"
#include "../structures/circular_queue.h"

#include <math.h>
#include <stdio.h>

#define CELLS 8
#define CELL_W 98.0f
#define CELL_PITCH 110.0f
#define CELL_X0 45.0f
#define CELL_Y 360.0f

static Queue linearQueue;
static CircularQueue circularQueue;
static int circular = 0;
static char buf[8];
static int active = 1;
static UIStatus st;
static int ops = 0;
static float scaleIn[CELLS];
static float frontX = 0.0f, rearX = 0.0f, ptrAlpha = 0.0f;

void QueueVisualizerInit(void)
{
    QueueInit(&linearQueue);
    CircularQueueInit(&circularQueue);
    circular = 0;
    buf[0] = '\0';
    active = 1;
    ops = 0;
    frontX = rearX = CELL_X0 + CELL_W / 2;
    ptrAlpha = 0.0f;

    for (int i = 0; i < CELLS; i++)
        scaleIn[i] = 1.0f;

    UISetStatus(&st, "Enter a value, then select ENQUEUE.", C_CYAN);
}

static int Count(void) { return circular ? circularQueue.count : linearQueue.count; }
static int Front(void) { return circular ? circularQueue.front : linearQueue.front; }
static int Rear(void) { return circular ? circularQueue.rear : linearQueue.rear; }
static int ValueAt(int i) { return circular ? circularQueue.items[i] : linearQueue.items[i]; }

static int Occupied(int index)
{
    if (circular)
    {
        for (int k = 0; k < circularQueue.count; k++)
            if ((circularQueue.front + k) % CIRCULAR_QUEUE_CAPACITY == index)
                return 1;
        return 0;
    }

    return linearQueue.count > 0 && index >= linearQueue.front && index <= linearQueue.rear;
}

static void DoEnqueue(void)
{
    int v;
    char msg[160];

    if (!UIParseInt(buf, &v))
    {
        UISetStatus(&st, "Enter a valid integer before ENQUEUE.", C_RED);
        return;
    }

    int ok = circular ? CircularQueueEnqueue(&circularQueue, v) : QueueEnqueue(&linearQueue, v);

    if (ok)
    {
        scaleIn[Rear()] = 0.0f;
        ops++;
        snprintf(msg, sizeof(msg), "ENQUEUE %d  ->  inserted at REAR (index %d).", v, Rear());
        UISetStatus(&st, msg, C_GREEN);
    }
    else if (!circular && linearQueue.count < QUEUE_CAPACITY)
        UISetStatus(&st, "OVERFLOW: REAR hit the end, yet FRONT freed slots. A circular queue fixes this!", C_AMBER);
    else
        UISetStatus(&st, "QUEUE OVERFLOW: the queue is full.", C_RED);

    buf[0] = '\0';
}

void QueueVisualizerDraw(void)
{
    UIBackground();
    UIHeader("QUEUE VISUALIZER", "FIFO  -  First In, First Out   |   linear and circular", C_PURPLE);

    UICard((Rectangle){30, 100, 890, 70}, NULL, C_PURPLE);
    if (UIButton((Rectangle){45, 112, 200, 46}, "LINEAR QUEUE", circular ? C_PANEL2 : C_CYAN))
    {
        circular = 0;
        UISetStatus(&st, "Linear queue selected: FRONT and REAR only move forward.", C_CYAN);
    }
    if (UIButton((Rectangle){255, 112, 220, 46}, "CIRCULAR QUEUE", circular ? C_PURPLE : C_PANEL2))
    {
        circular = 1;
        UISetStatus(&st, "Circular queue selected: indices wrap with % capacity.", C_PURPLE);
    }

    UICard((Rectangle){30, 185, 890, 100}, "OPERATIONS", C_PURPLE);

    if (UIInput((Rectangle){50, 222, 180, 50}, buf, sizeof(buf), &active, "Enter value", 0))
        DoEnqueue();

    if (UIButton((Rectangle){245, 222, 130, 50}, "ENQUEUE", C_CYAN))
        DoEnqueue();

    if (UIButton((Rectangle){385, 222, 130, 50}, "DEQUEUE", C_PURPLE))
    {
        int removed = 0;
        char msg[120];
        int ok = circular ? CircularQueueDequeue(&circularQueue, &removed)
                          : QueueDequeue(&linearQueue, &removed);

        if (ok)
        {
            ops++;
            snprintf(msg, sizeof(msg), "DEQUEUE removed %d from FRONT.", removed);
            UISetStatus(&st, msg, C_GREEN);
        }
        else
            UISetStatus(&st, "QUEUE UNDERFLOW: the queue is empty.", C_RED);
    }

    if (UIButton((Rectangle){525, 222, 130, 50}, "PEEK", C_BLUE))
    {
        int fv = 0;
        char msg[100];
        int ok = circular ? CircularQueuePeek(&circularQueue, &fv) : QueuePeek(&linearQueue, &fv);

        if (ok)
        {
            snprintf(msg, sizeof(msg), "PEEK: the FRONT element is %d.", fv);
            UISetStatus(&st, msg, C_CYAN);
        }
        else
            UISetStatus(&st, "Cannot PEEK: the queue is empty.", C_RED);
    }

    if (UIButton((Rectangle){665, 222, 130, 50}, "CLEAR", C_RED))
    {
        QueueInit(&linearQueue);
        CircularQueueInit(&circularQueue);
        buf[0] = '\0';
        ops++;
        UISetStatus(&st, "Queue cleared.", C_AMBER);
    }

    DrawText(circular ? "CIRCULAR QUEUE MEMORY" : "LINEAR QUEUE MEMORY", 45, 312, 18, C_WHITE);

    int count = Count();
    DrawText(TextFormat("SIZE  %d / %d", count, CELLS), 770, 312, 16, C_CYAN);

    DrawText(TextFormat("FRONT = %d     REAR = %d", count ? Front() : -1, count ? Rear() : -1),
             45, 338, 15, C_MUTED);

    /* animated FRONT / REAR pointers */
    float targetF = CELL_X0 + Front() * CELL_PITCH + CELL_W / 2;
    float targetR = CELL_X0 + (Rear() < 0 ? 0 : Rear()) * CELL_PITCH + CELL_W / 2;
    frontX = UIApproach(frontX, targetF, 10.0f);
    rearX = UIApproach(rearX, targetR, 10.0f);
    ptrAlpha = UIApproach(ptrAlpha, count > 0 ? 1.0f : 0.0f, 8.0f);

    for (int i = 0; i < CELLS; i++)
    {
        float x = CELL_X0 + i * CELL_PITCH;
        int occupied = Occupied(i);
        int wasted = !circular && !occupied && linearQueue.count > 0 && i < linearQueue.front;

        if (occupied)
            scaleIn[i] = UIApproach(scaleIn[i], 1.0f, 12.0f);

        float sc = occupied ? scaleIn[i] : 1.0f;
        Rectangle r = {x + CELL_W * (1 - sc) / 2, CELL_Y + 90 * (1 - sc) / 2, CELL_W * sc, 90 * sc};

        Color fill = occupied ? ((i == Front()) ? C_PURPLE : C_PANEL2) : Fade(C_PANEL, 0.7f);
        Color border = occupied ? C_CYAN : (wasted ? Fade(C_AMBER, 0.6f) : Fade(C_BORDER, 0.8f));

        UICell(r, occupied ? TextFormat("%d", ValueAt(i)) : (wasted ? "FREED" : "EMPTY"),
               fill, border, occupied ? 22 : 12);

        DrawText(TextFormat("[%d]", i), (int)x + 36, (int)CELL_Y + 98, 13, C_MUTED);
    }

    if (ptrAlpha > 0.02f)
    {
        UIArrow((Vector2){frontX, CELL_Y - 38}, (Vector2){frontX, CELL_Y - 4}, Fade(C_CYAN, ptrAlpha), 3.0f);
        DrawText("FRONT", (int)frontX - 24, (int)CELL_Y - 58, 14, Fade(C_CYAN, ptrAlpha));

        UIArrow((Vector2){rearX, CELL_Y + 152}, (Vector2){rearX, CELL_Y + 118}, Fade(C_GREEN, ptrAlpha), 3.0f);
        DrawText("REAR", (int)rearX - 18, (int)CELL_Y + 156, 14, Fade(C_GREEN, ptrAlpha));
    }

    UICard((Rectangle){30, 545, 890, 78}, "HOW IT WORKS", C_PURPLE);
    DrawText(circular ? "REAR = (REAR + 1) % 8 and FRONT = (FRONT + 1) % 8  -  freed slots are reused."
                      : "REAR++ on enqueue, FRONT++ on dequeue  -  freed slots at the front are never reused.",
             56, 578, 16, C_WHITE);
    DrawText(TextFormat("Operations performed: %d", ops), 56, 600, 13, C_MUTED);

    UIStatusBar(&st, (Rectangle){30, 645, 890, 80});
}
