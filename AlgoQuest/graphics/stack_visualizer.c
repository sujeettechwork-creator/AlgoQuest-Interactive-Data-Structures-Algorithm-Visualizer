#include "stack_visualizer.h"
#include "../structures/stack.h"

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const Color UI_BG = {15, 18, 32, 255};
static const Color UI_PANEL = {25, 32, 51, 255};
static const Color UI_CARD = {32, 41, 64, 255};
static const Color UI_CYAN = {0, 210, 220, 255};
static const Color UI_PURPLE = {150, 100, 255, 255};
static const Color UI_WHITE = {240, 245, 255, 255};
static const Color UI_MUTED = {145, 157, 180, 255};
static const Color UI_GREEN = {60, 210, 130, 255};
static const Color UI_RED = {245, 90, 105, 255};

static Stack stackData;
static char inputBuffer[12];
static int inputLength = 0;
static char statusMessage[100] = "Enter a number and press PUSH.";
static Color statusColor;
static bool inputActive = true;
static bool returnToDashboard = false;
static int operationCount = 0;
static int lastAction = 0; /* 1 = push, 2 = pop, 3 = peek */

// Button click check for Update logic
static bool IsButtonClicked(Rectangle rect)
{
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, rect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Button drawing for Draw logic
static void DrawButton(Rectangle rect, const char *label, Color color)
{
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rect);
    DrawRectangleRounded(rect, 0.18f, 8,
                         hover ? ColorBrightness(color, 0.18f) : color);
    int fontSize = 18;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label, (int)(rect.x + (rect.width - textWidth) / 2),
             (int)(rect.y + (rect.height - fontSize) / 2),
             fontSize, UI_WHITE);
}

static void SetStatus(const char *message, Color color)
{
    snprintf(statusMessage, sizeof(statusMessage), "%s", message);
    statusColor = color;
}

void StackVisualizerInit(void)
{
    StackInit(&stackData);
    inputBuffer[0] = '\0';
    inputLength = 0;
    operationCount = 0;
    lastAction = 0;
    returnToDashboard = false;
    inputActive = true;
    statusColor = UI_CYAN;
    SetStatus("Enter a number and press PUSH.", UI_CYAN);
}

void StackVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnToDashboard = true;
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Rectangle inputRect = {320, 205, 180, 48};
        inputActive = CheckCollisionPointRec(GetMousePosition(), inputRect);
    }

    if (inputActive)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= '0' && key <= '9') ||
                (key == '-' && inputLength == 0))
            {
                if (inputLength < (int)sizeof(inputBuffer) - 1)
                {
                    inputBuffer[inputLength++] = (char)key;
                    inputBuffer[inputLength] = '\0';
                }
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputLength > 0)
        {
            inputLength--;
            inputBuffer[inputLength] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER) && inputLength > 0)
        {
            int value = atoi(inputBuffer);
            if (StackPush(&stackData, value))
            {
                operationCount++;
                lastAction = 1;
                SetStatus("PUSH successful: element added.", UI_GREEN);
            }
            else
            {
                SetStatus("STACK OVERFLOW: stack is full.", UI_RED);
            }
            inputBuffer[0] = '\0';
            inputLength = 0;
        }
    }

    if (IsButtonClicked((Rectangle){530, 205, 125, 48}))
    {
        if (inputLength > 0)
        {
            int value = atoi(inputBuffer);
            if (StackPush(&stackData, value))
            {
                operationCount++;
                lastAction = 1;
                SetStatus("PUSH successful: element added.", UI_GREEN);
            }
            else
                SetStatus("STACK OVERFLOW: stack is full.", UI_RED);

            inputBuffer[0] = '\0';
            inputLength = 0;
        }
        else
            SetStatus("Enter a number before pressing PUSH.", UI_RED);
    }

    if (IsButtonClicked((Rectangle){675, 205, 125, 48}))
    {
        int removed;
        if (StackPop(&stackData, &removed))
        {
            char message[100];
            snprintf(message, sizeof(message), "POP successful: removed %d.", removed);
            operationCount++;
            lastAction = 2;
            SetStatus(message, UI_GREEN);
        }
        else
            SetStatus("STACK UNDERFLOW: stack is empty.", UI_RED);
    }

    if (IsButtonClicked((Rectangle){820, 205, 125, 48}))
    {
        int topValue;
        if (StackPeek(&stackData, &topValue))
        {
            char message[100];
            snprintf(message, sizeof(message), "PEEK: top element is %d.", topValue);
            lastAction = 3;
            SetStatus(message, UI_CYAN);
        }
        else
            SetStatus("Cannot PEEK: stack is empty.", UI_RED);
    }

    if (IsButtonClicked((Rectangle){965, 205, 125, 48}))
    {
        StackInit(&stackData);
        inputBuffer[0] = '\0';
        inputLength = 0;
        lastAction = 0;
        operationCount++;
        SetStatus("Stack cleared.", UI_CYAN);
    }

    if (IsButtonClicked((Rectangle){30, 30, 150, 42}))
        returnToDashboard = true;
}

void StackVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    DrawText("STACK VISUALIZER", 220, 35, 30, UI_CYAN);
    DrawText("Array implementation | LIFO: Last In, First Out",
             220, 78, 16, UI_MUTED);

    DrawRectangleRounded((Rectangle){210, 130, 910, 150}, 0.04f, 8, UI_PANEL);
    DrawText("STACK OPERATIONS", 240, 150, 16, UI_WHITE);

    // Draw Operation Buttons
    DrawButton((Rectangle){530, 205, 125, 48}, "PUSH", UI_CYAN);
    DrawButton((Rectangle){675, 205, 125, 48}, "POP", UI_PURPLE);
    DrawButton((Rectangle){820, 205, 125, 48}, "PEEK", (Color){65, 105, 150, 255});
    DrawButton((Rectangle){965, 205, 125, 48}, "CLEAR", (Color){115, 65, 85, 255});
    DrawButton((Rectangle){30, 30, 150, 42}, "BACK", (Color){55, 65, 90, 255});

    DrawRectangleRounded((Rectangle){320, 205, 180, 48}, 0.15f, 8,
                         inputActive ? (Color){45, 60, 85, 255} : UI_CARD);
    
    DrawText(inputBuffer[0] ? inputBuffer : "Enter value",
             335, 219, 18, inputBuffer[0] ? UI_WHITE : UI_MUTED);

    DrawText("INPUT", 320, 185, 11, UI_MUTED);

    DrawText("Use the input box, type a number, then press PUSH or ENTER.",
             320, 260, 12, UI_MUTED);

    DrawText("STACK MEMORY", 220, 315, 18, UI_WHITE);
    DrawText(TextFormat("SIZE: %d / %d", StackSize(&stackData), STACK_CAPACITY),
             900, 315, 16, UI_CYAN);

    const int cellX = 450;
    const int cellY = 335;
    const int cellW = 260;
    const int cellH = 34;
    const int gap = 5;

    for (int i = 0; i < STACK_CAPACITY; i++)
    {
        int y = cellY + (STACK_CAPACITY - 1 - i) * (cellH + gap);
        bool occupied = i <= stackData.top;
        bool isTop = occupied && i == stackData.top;
        Color cellColor = isTop ? UI_PURPLE :
                          (occupied ? UI_CARD : (Color){21, 27, 43, 255});

        DrawRectangleRounded((Rectangle){cellX, y, cellW, cellH},
                             0.12f, 6, cellColor);
        DrawRectangleLinesEx((Rectangle){cellX, y, cellW, cellH},
                             1, occupied ? UI_CYAN : (Color){55, 65, 85, 255});

        if (occupied)
            DrawText(TextFormat("%d", stackData.items[i]),
                     cellX + 115, y + 10, 20, UI_WHITE);
        else
            DrawText("EMPTY", cellX + 105, y + 13, 14, UI_MUTED);

        DrawText(TextFormat("[%d]", i), cellX - 55, y + 13, 14, UI_MUTED);

        if (isTop)
            DrawText("TOP  <-", cellX + cellW + 20, y + 13, 15, UI_CYAN);
    }

    DrawRectangleRounded((Rectangle){760, 335, 330, 180}, 0.05f, 8, UI_PANEL);
    DrawText("STACK INFORMATION", 785, 390, 17, UI_WHITE);
    DrawText("Rule: LIFO", 785, 430, 16, UI_CYAN);
    DrawText(TextFormat("Top index: %d", stackData.top),
             785, 462, 15, UI_WHITE);
    DrawText(TextFormat("Operations: %d", operationCount),
             785, 490, 15, UI_WHITE);
    DrawText("Capacity: 8 elements", 785, 518, 14, UI_MUTED);

    DrawRectangleRounded((Rectangle){220, 665, 870, 65},
                         0.08f, 8, UI_PANEL);
    DrawText("STATUS", 240, 680, 12, UI_MUTED);
    DrawText(statusMessage, 240, 702, 17, statusColor);

    DrawText("BACK or ESC: return to dashboard",
             850, 35, 13, UI_MUTED);
}

void StackVisualizerClose(void)
{
    /* No dynamically allocated memory is used in this module. */
}

int StackVisualizerWantsDashboard(void)
{
    if (returnToDashboard)
    {
        returnToDashboard = false;
        return 1;
    }
    return 0;
}