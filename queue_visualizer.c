#include "queue_visualizer.h"
#include "../structures/queue.h"
#include "../structures/circular_queue.h"

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

static const Color UI_BG = {15, 18, 32, 255};
static const Color UI_PANEL = {25, 32, 51, 255};
static const Color UI_CELL = {32, 41, 64, 255};
static const Color UI_CYAN = {0, 210, 220, 255};
static const Color UI_PURPLE = {150, 100, 255, 255};
static const Color UI_WHITE = {240, 245, 255, 255};
static const Color UI_MUTED = {145, 157, 180, 255};
static const Color UI_GREEN = {60, 210, 130, 255};
static const Color UI_RED = {245, 90, 105, 255};

static Queue linearQueue;
static CircularQueue circularQueue;
static int circularMode = 0;
static char inputBuffer[16];
static int inputLength = 0;
static int inputActive = 1;
static int returnToDashboard = 0;
static int operationCount = 0;
static char statusMessage[120] = "Enter a value, then select ENQUEUE.";
static Color statusColor; // Fixed: non-constant initializers cannot be used for static objects at file scope

static int Hit(Rectangle r)
{
    return CheckCollisionPointRec(GetMousePosition(), r) &&
           IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void SetStatus(const char *message, Color color)
{
    snprintf(statusMessage, sizeof(statusMessage), "%s", message);
    statusColor = color;
}

static void DrawButton(Rectangle r, const char *label, Color color, int active)
{
    int hover = CheckCollisionPointRec(GetMousePosition(), r);
    Color fill = active ? color : (hover ? ColorBrightness(color, 0.18f) : color);
    DrawRectangleRounded(r, 0.16f, 8, fill);
    int fs = 17;
    DrawText(label, (int)(r.x + (r.width - MeasureText(label, fs)) / 2),
             (int)(r.y + (r.height - fs) / 2), fs, UI_WHITE);
}

static int IsOccupied(int index)
{
    if (circularMode)
    {
        for (int k = 0; k < circularQueue.count; k++)
            if ((circularQueue.front + k) % CIRCULAR_QUEUE_CAPACITY == index)
                return 1;
        return 0;
    }

    return index >= linearQueue.front &&
           index <= linearQueue.rear &&
           !QueueIsEmpty(&linearQueue);
}

static int ValueAt(int index)
{
    return circularMode ? circularQueue.items[index] : linearQueue.items[index];
}

void QueueVisualizerInit(void)
{
    QueueInit(&linearQueue);
    CircularQueueInit(&circularQueue);
    circularMode = 0;
    inputBuffer[0] = '\0';
    inputLength = 0;
    inputActive = 1;
    returnToDashboard = 0;
    operationCount = 0;
    SetStatus("Enter a value, then select ENQUEUE.", UI_CYAN);
}

void QueueVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnToDashboard = 1;
        return;
    }

    if (Hit((Rectangle){30, 25, 130, 42}))
        returnToDashboard = 1;

    if (Hit((Rectangle){220, 130, 190, 42}))
    {
        circularMode = 0;
        inputBuffer[0] = '\0';
        inputLength = 0;
        SetStatus("Linear Queue selected.", UI_CYAN);
    }

    if (Hit((Rectangle){425, 130, 210, 42}))
    {
        circularMode = 1;
        inputBuffer[0] = '\0';
        inputLength = 0;
        SetStatus("Circular Queue selected.", UI_CYAN);
    }

    Rectangle inputRect = {220, 220, 180, 48};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        inputActive = CheckCollisionPointRec(GetMousePosition(), inputRect);

    if (inputActive)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if (((key >= '0' && key <= '9') ||
                 (key == '-' && inputLength == 0)) &&
                inputLength < (int)sizeof(inputBuffer) - 1)
            {
                inputBuffer[inputLength++] = (char)key;
                inputBuffer[inputLength] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputLength > 0)
        {
            inputBuffer[--inputLength] = '\0';
        }
    }

    if (Hit((Rectangle){420, 220, 135, 48}))
    {
        if (inputLength == 0)
            SetStatus("Enter a number before ENQUEUE.", UI_RED);
        else
        {
            int value = atoi(inputBuffer);
            int ok = circularMode
                ? CircularQueueEnqueue(&circularQueue, value)
                : QueueEnqueue(&linearQueue, value);

            if (ok)
            {
                operationCount++;
                SetStatus("ENQUEUE successful: value inserted at REAR.", UI_GREEN);
            }
            else
                SetStatus("QUEUE OVERFLOW: queue is full.", UI_RED);

            inputBuffer[0] = '\0';
            inputLength = 0;
        }
    }

    if (Hit((Rectangle){575, 220, 135, 48}))
    {
        int removed = 0;
        int ok = circularMode
            ? CircularQueueDequeue(&circularQueue, &removed)
            : QueueDequeue(&linearQueue, &removed);

        if (ok)
        {
            char msg[120];
            snprintf(msg, sizeof(msg), "DEQUEUE successful: removed %d from FRONT.", removed);
            operationCount++;
            SetStatus(msg, UI_GREEN);
        }
        else
            SetStatus("QUEUE UNDERFLOW: queue is empty.", UI_RED);
    }

    if (Hit((Rectangle){730, 220, 135, 48}))
    {
        int frontValue = 0;
        int ok = circularMode
            ? CircularQueuePeek(&circularQueue, &frontValue)
            : QueuePeek(&linearQueue, &frontValue);

        if (ok)
        {
            char msg[120];
            snprintf(msg, sizeof(msg), "PEEK: FRONT element is %d.", frontValue);
            SetStatus(msg, UI_CYAN);
        }
        else
            SetStatus("Cannot PEEK: queue is empty.", UI_RED);
    }

    if (Hit((Rectangle){885, 220, 135, 48}))
    {
        QueueInit(&linearQueue);
        CircularQueueInit(&circularQueue);
        inputBuffer[0] = '\0';
        inputLength = 0;
        operationCount++;
        SetStatus("Queue cleared.", UI_CYAN); // Fixed typo
    }
}

void QueueVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    DrawText("QUEUE VISUALIZER", 220, 35, 30, UI_CYAN);
    DrawText("FIFO: First In, First Out", 220, 78, 16, UI_MUTED);
    DrawButton((Rectangle){30, 25, 130, 42}, "BACK", (Color){55, 65, 90, 255}, 0);

    DrawRectangleRounded((Rectangle){205, 115, 910, 80}, 0.04f, 8, UI_PANEL);
    DrawButton((Rectangle){220, 130, 190, 42}, "LINEAR QUEUE", circularMode ? UI_CELL : UI_CYAN, !circularMode);
    DrawButton((Rectangle){425, 130, 210, 42}, "CIRCULAR QUEUE", circularMode ? UI_PURPLE : UI_CELL, circularMode);

    DrawRectangleRounded((Rectangle){205, 205, 910, 95}, 0.04f, 8, UI_PANEL);
    DrawText("INPUT", 220, 207, 11, UI_MUTED);
    DrawRectangleRounded((Rectangle){220, 220, 180, 48}, 0.14f, 8, UI_CELL);
    DrawText(inputBuffer[0] ? inputBuffer : "Enter value", 235, 234, 18,
             inputBuffer[0] ? UI_WHITE : UI_MUTED);
    DrawButton((Rectangle){420, 220, 135, 48}, "ENQUEUE", UI_CYAN, 0);
    DrawButton((Rectangle){575, 220, 135, 48}, "DEQUEUE", UI_PURPLE, 0);
    DrawButton((Rectangle){730, 220, 135, 48}, "PEEK", (Color){65, 105, 150, 255}, 0);
    DrawButton((Rectangle){885, 220, 135, 48}, "CLEAR", (Color){115, 65, 85, 255}, 0);

    DrawText(circularMode ? "CIRCULAR QUEUE MEMORY" : "LINEAR QUEUE MEMORY",
             220, 330, 18, UI_WHITE);
    int count = circularMode ? CircularQueueSize(&circularQueue) : QueueSize(&linearQueue);
    int front = circularMode ? circularQueue.front : linearQueue.front;
    int rear = circularMode ? circularQueue.rear : linearQueue.rear;

    DrawText(TextFormat("SIZE: %d / 8", count), 900, 330, 16, UI_CYAN);
    DrawText(TextFormat("FRONT: %d    REAR: %d", count ? front : -1,
                        count ? rear : -1), 220, 365, 15, UI_MUTED);

    int startX = 235;
    int y = 430;
    int UI_CELLW = 95;
    int gap = 13;

    for (int i = 0; i < 8; i++)
    {
        int x = startX + i * (UI_CELLW + gap);
        int occupied = IsOccupied(i);
        Color fill = occupied ? ((i == front) ? UI_PURPLE : UI_CELL) : (Color){21, 27, 43, 255};
        DrawRectangleRounded((Rectangle){x, y, UI_CELLW, 75}, 0.12f, 7, fill);
        DrawRectangleLinesEx((Rectangle){x, y, UI_CELLW, 75}, 1, occupied ? UI_CYAN : UI_MUTED);
        DrawText(occupied ? TextFormat("%d", ValueAt(i)) : "EMPTY",
                 x + 8, y + 26, occupied ? 20 : 12, UI_WHITE);
        DrawText(TextFormat("[%d]", i), x + 35, y + 83, 12, UI_MUTED);
        if (occupied && i == front)
            DrawText("FRONT", x + 10, y - 24, 12, UI_CYAN);
        if (occupied && i == rear)
            DrawText("REAR", x + 12, y + 105, 12, UI_GREEN);
    }

    DrawRectangleRounded((Rectangle){205, 585, 910, 105}, 0.05f, 8, UI_PANEL);
    DrawText("STATUS", 225, 600, 12, UI_MUTED);
    DrawText(statusMessage, 225, 625, 17, statusColor);
    DrawText(TextFormat("Operations: %d", operationCount), 225, 657, 14, UI_WHITE);
    DrawText(circularMode ? "Circular indexing uses (index + 1) %% capacity."
                          : "Linear queue advances FRONT and REAR without wrap-around.",
             510, 657, 13, UI_MUTED);
}

int QueueVisualizerWantsDashboard(void)
{
    if (returnToDashboard)
    {
        returnToDashboard = 0;
        return 1;
    }
    return 0;
}
