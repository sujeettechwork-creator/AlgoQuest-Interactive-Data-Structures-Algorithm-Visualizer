/* graphics/heap_visualizer.c */
#include "heap_visualizer.h"
#include "../structures/heap.h"

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef UI_DARKGRAY
#define UI_DARKGRAY DARKGRAY
#endif

static Heap heap;
static char input[16] = "";
static int inputLength = 0;
static char message[100] = "Enter a value and select an operation.";
static Color messageColor;

static Rectangle insertButton = {40, 145, 120, 42};
static Rectangle extractButton = {175, 145, 145, 42};
static Rectangle peekButton = {335, 145, 100, 42};
static Rectangle clearButton = {450, 145, 100, 42};
static Rectangle modeButton = {570, 145, 150, 42};

static int IsMouseOver(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

static void SetMessage(const char *text, Color color)
{
    snprintf(message, sizeof(message), "%s", text);
    messageColor = color;
}

static void DrawButton(Rectangle rect, const char *label)
{
    Color color = IsMouseOver(rect) ? SKYBLUE : LIGHTGRAY;

    DrawRectangleRec(rect, color);
    DrawRectangleLinesEx(rect, 1, UI_DARKGRAY);

    int textWidth = MeasureText(label, 17);
    DrawText(label,
             (int)(rect.x + (rect.width - textWidth) / 2),
             (int)(rect.y + 12),
             17, BLACK);
}

static void DrawHeapNode(int index, float x, float y,
                         float spread, int depth)
{
    if (index >= heap.size || depth > 4)
        return;

    int left = 2 * index + 1;
    int right = 2 * index + 2;

    float childY = y + 85;
    float childSpread = spread / 2.0f;

    if (left < heap.size) {
        float childX = x - spread;
        DrawLineEx((Vector2){x, y + 22},
                   (Vector2){childX, childY}, 2, GRAY);
        DrawHeapNode(left, childX, childY, childSpread, depth + 1);
    }

    if (right < heap.size) {
        float childX = x + spread;
        DrawLineEx((Vector2){x, y + 22},
                   (Vector2){childX, childY}, 2, GRAY);
        DrawHeapNode(right, childX, childY, childSpread, depth + 1);
    }

    DrawCircle((int)x, (int)y, 23, (depth == 0) ? GOLD : SKYBLUE);
    DrawCircleLines((int)x, (int)y, 23, DARKBLUE);

    char value[16];
    snprintf(value, sizeof(value), "%d", heap.data[index]);

    int width = MeasureText(value, 18);
    DrawText(value, (int)x - width / 2, (int)y - 9, 18, BLACK);
}

void HeapVisualizerInit(void)
{
    HeapInit(&heap, 0);
    input[0] = '\0';
    inputLength = 0;
    messageColor = UI_DARKGRAY;
    SetMessage("Max Heap ready. Enter a value.", UI_DARKGRAY);
}

void HeapVisualizerUpdate(void)
{
    int key = GetCharPressed();

    while (key > 0) {
        if (key >= '0' && key <= '9' && inputLength < 14) {
            input[inputLength++] = (char)key;
            input[inputLength] = '\0';
        }
        else if (key == '-' && inputLength == 0) {
            input[inputLength++] = '-';
            input[inputLength] = '\0';
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && inputLength > 0) {
        inputLength--;
        input[inputLength] = '\0';
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    if (IsMouseOver(insertButton)) {
        if (inputLength == 0 || strcmp(input, "-") == 0) {
            SetMessage("Enter a valid integer first.", RED);
        } else {
            int value = atoi(input);

            if (HeapInsert(&heap, value))
                SetMessage("Value inserted successfully.", GREEN);
            else
                SetMessage("Heap is full!", RED);

            input[0] = '\0';
            inputLength = 0;
        }
    }
    else if (IsMouseOver(extractButton)) {
        int value;

        if (HeapExtractRoot(&heap, &value)) {
            char text[100];
            snprintf(text, sizeof(text), "Extracted root: %d", value);
            SetMessage(text, GREEN);
        } else {
            SetMessage("Heap is empty!", RED);
        }
    }
    else if (IsMouseOver(peekButton)) {
        int value;

        if (HeapPeek(&heap, &value)) {
            char text[100];
            snprintf(text, sizeof(text), "Root value: %d", value);
            SetMessage(text, GREEN);
        } else {
            SetMessage("Heap is empty!", RED);
        }
    }
    else if (IsMouseOver(clearButton)) {
        HeapClear(&heap);
        SetMessage("Heap cleared.", GREEN);
    }
    else if (IsMouseOver(modeButton)) {
        int newMode = !heap.isMinHeap;

        HeapClear(&heap);
        HeapInit(&heap, newMode);

        SetMessage("Heap mode changed. Previous values cleared.", ORANGE);
    }
}

int HeapVisualizerWantsDashboard(void)
{
    return 0;
}

void HeapVisualizerDraw(void)
{
    const char *mode = heap.isMinHeap ? "MIN HEAP" : "MAX HEAP";

    DrawText("HEAP & PRIORITY QUEUE VISUALIZER",
             40, 35, 27, DARKBLUE);

    DrawText(mode, 40, 85, 22, MAROON);

    DrawText("Value:", 40, 112, 18, UI_DARKGRAY);
    DrawRectangle(105, 108, 100, 27, RAYWHITE);
    DrawRectangleLines(105, 108, 100, 27, GRAY);
    DrawText(input, 112, 112, 18, BLACK);

    DrawButton(insertButton, "INSERT");
    DrawButton(extractButton, "EXTRACT ROOT");
    DrawButton(peekButton, "PEEK");
    DrawButton(clearButton, "CLEAR");
    DrawButton(modeButton, "TOGGLE MODE");

    DrawText(message, 40, 205, 18, messageColor);

    DrawText("HEAP TREE", 40, 250, 20, DARKBLUE);

    if (heap.size == 0) {
        DrawText("Heap is empty. Insert values to build the tree.",
                 40, 300, 18, GRAY);
    } else {
        DrawHeapNode(0, 600, 310, 250, 0);
    }

    DrawText("ARRAY REPRESENTATION", 40, 590, 18, DARKBLUE);

    int visible = heap.size < 15 ? heap.size : 15;

    for (int i = 0; i < visible; i++) {
        int x = 40 + i * 73;

        DrawRectangle(x, 625, 62, 38, LIGHTGRAY);
        DrawRectangleLines(x, 625, 62, 38, UI_DARKGRAY);

        char value[16];
        snprintf(value, sizeof(value), "%d", heap.data[i]);

        int width = MeasureText(value, 17);
        DrawText(value, x + (62 - width) / 2, 636, 17, BLACK);

        char index[12];
        snprintf(index, sizeof(index), "%d", i);
        DrawText(index, x + 25, 668, 12, GRAY);
    }

    char sizeText[50];
    snprintf(sizeText, sizeof(sizeText),
             "Heap Size: %d / %d", heap.size, HEAP_CAPACITY);

    DrawText(sizeText, 850, 85, 18, UI_DARKGRAY);

    DrawText("Priority Queue: Extract Root removes the highest-priority item.",
             40, 685, 14, GRAY);
}