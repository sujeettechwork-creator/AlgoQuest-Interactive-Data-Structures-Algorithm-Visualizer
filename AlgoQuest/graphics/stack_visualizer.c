#include "stack_visualizer.h"
#include "ui.h"
#include "../structures/stack.h"

#include <math.h>
#include <stdio.h>

static Stack stack;
static char buf[8];
static int active = 1;
static UIStatus st;
static int ops = 0;
static float scaleIn[STACK_CAPACITY];
static float ghostA = 0.0f;
static int ghostVal = 0;
static int ghostIdx = 0;
static float peekFlash = 0.0f;

void StackVisualizerInit(void)
{
    StackInit(&stack);
    buf[0] = '\0';
    active = 1;
    ops = 0;
    ghostA = 0.0f;
    peekFlash = 0.0f;

    for (int i = 0; i < STACK_CAPACITY; i++)
        scaleIn[i] = 1.0f;

    UISetStatus(&st, "Type a number and press PUSH (or Enter).", C_CYAN);
}

static void DoPush(void)
{
    int v;
    char msg[120];

    if (!UIParseInt(buf, &v))
    {
        UISetStatus(&st, "Enter a valid integer before PUSH.", C_RED);
        return;
    }

    if (StackPush(&stack, v))
    {
        scaleIn[stack.top] = 0.0f;
        ops++;
        snprintf(msg, sizeof(msg), "PUSH %d  ->  TOP moved to index %d.", v, stack.top);
        UISetStatus(&st, msg, C_GREEN);
    }
    else
        UISetStatus(&st, "STACK OVERFLOW: the stack is full (capacity 8).", C_RED);

    buf[0] = '\0';
}

static void DoPop(void)
{
    int removed;
    char msg[120];

    if (!StackIsEmpty(&stack))
    {
        ghostIdx = stack.top;
        StackPop(&stack, &removed);
        ghostVal = removed;
        ghostA = 1.0f;
        ops++;
        snprintf(msg, sizeof(msg), "POP removed %d from the top.", removed);
        UISetStatus(&st, msg, C_GREEN);
    }
    else
        UISetStatus(&st, "STACK UNDERFLOW: the stack is empty.", C_RED);
}

void StackVisualizerDraw(void)
{
    UIBackground();
    UIHeader("STACK VISUALIZER", "LIFO  -  Last In, First Out   |   array implementation", C_CYAN);

    UICard((Rectangle){30, 100, 890, 110}, "OPERATIONS", C_CYAN);

    if (UIInput((Rectangle){50, 142, 190, 50}, buf, sizeof(buf), &active, "Enter value", 0))
        DoPush();

    if (UIButton((Rectangle){260, 142, 120, 50}, "PUSH", C_CYAN))
        DoPush();

    if (UIButton((Rectangle){390, 142, 120, 50}, "POP", C_PURPLE))
        DoPop();

    if (UIButton((Rectangle){520, 142, 120, 50}, "PEEK", C_BLUE))
    {
        int top;
        char msg[100];

        if (StackPeek(&stack, &top))
        {
            peekFlash = 1.0f;
            snprintf(msg, sizeof(msg), "PEEK: the top element is %d (not removed).", top);
            UISetStatus(&st, msg, C_CYAN);
        }
        else
            UISetStatus(&st, "Cannot PEEK: the stack is empty.", C_RED);
    }

    if (UIButton((Rectangle){650, 142, 120, 50}, "CLEAR", C_RED))
    {
        StackInit(&stack);
        buf[0] = '\0';
        ops++;
        UISetStatus(&st, "Stack cleared.", C_AMBER);
    }

    DrawText("Press Enter inside the box to push quickly.", 790 - 20, 150, 12, C_MUTED);

    DrawText("STACK MEMORY", 60, 232, 18, C_WHITE);
    DrawText(TextFormat("SIZE  %d / %d", StackSize(&stack), STACK_CAPACITY), 780, 232, 16, C_CYAN);

    peekFlash = UIApproach(peekFlash, 0.0f, 2.5f);
    ghostA = UIApproach(ghostA, 0.0f, 4.0f);

    for (int i = 0; i < STACK_CAPACITY; i++)
    {
        float y = 265.0f + (STACK_CAPACITY - 1 - i) * 46.0f;
        int occupied = i <= stack.top;
        int isTop = occupied && i == stack.top;

        if (occupied)
            scaleIn[i] = UIApproach(scaleIn[i], 1.0f, 12.0f);

        float sc = occupied ? scaleIn[i] : 1.0f;
        Rectangle r = {300.0f + 270.0f * (1.0f - sc) / 2.0f, y - (1.0f - sc) * 40.0f,
                       270.0f * sc, 38.0f};

        Color fill = isTop ? UILerpColor(C_PURPLE, C_AMBER, peekFlash)
                           : (occupied ? C_PANEL2 : Fade(C_PANEL, 0.7f));
        Color border = occupied ? C_CYAN : Fade(C_BORDER, 0.8f);

        if (isTop)
            DrawRectangleRounded((Rectangle){r.x - 6, r.y - 6, r.width + 12, r.height + 12},
                                 0.3f, 8, Fade(C_PURPLE, 0.20f + 0.10f * sinf(UITime() * 4.0f)));

        UICell(r, occupied ? TextFormat("%d", stack.items[i]) : "EMPTY", fill, border,
               occupied ? 20 : 13);

        DrawText(TextFormat("[%d]", i), 250, (int)y + 11, 14, C_MUTED);

        if (isTop)
            DrawText("<-  TOP", 590 + (int)(4.0f * sinf(UITime() * 5.0f)), (int)y + 10, 16, C_CYAN);
    }

    if (ghostA > 0.02f)
    {
        float y = 265.0f + (STACK_CAPACITY - 1 - ghostIdx) * 46.0f - (1.0f - ghostA) * 60.0f;
        UICell((Rectangle){300, y, 270, 38}, TextFormat("%d", ghostVal),
               Fade(C_PURPLE, ghostA), Fade(C_WHITE, ghostA), 20);
    }

    UICard((Rectangle){650, 265, 270, 215}, "STACK INFORMATION", C_PURPLE);
    DrawText("Rule: LIFO", 672, 305, 17, C_CYAN);
    DrawText(TextFormat("Top index:  %d", stack.top), 672, 337, 16, C_WHITE);
    DrawText(TextFormat("Operations: %d", ops), 672, 365, 16, C_WHITE);
    DrawText(StackIsEmpty(&stack) ? "State: EMPTY" : (StackIsFull(&stack) ? "State: FULL" : "State: OK"),
             672, 393, 16, StackIsFull(&stack) ? C_RED : C_GREEN);

    float fill = (float)StackSize(&stack) / STACK_CAPACITY;
    DrawRectangleRounded((Rectangle){672, 430, 226, 14}, 0.5f, 6, C_BG);
    if (fill > 0.0f)
        DrawRectangleRounded((Rectangle){672, 430, 226 * fill, 14}, 0.5f, 6,
                             UILerpColor(C_GREEN, C_RED, fill));
    DrawText("Capacity: 8 elements", 672, 452, 13, C_MUTED);

    UIStatusBar(&st, (Rectangle){30, 655, 890, 80});
}
