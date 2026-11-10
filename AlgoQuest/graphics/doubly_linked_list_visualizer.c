#include "doubly_linked_list_visualizer.h"
#include "../structures/doubly_linked_list.h"

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_W 1200
#define SCREEN_H 700
#define NODE_LIMIT 6

static DoublyLinkedList dll;
static char input[32] = "";
static int inputLength = 0;
static int inputActive = 0;
static int returnDashboard = 0;
static int scrollOffset = 0;
static char message[100] = "Enter a value to begin.";

static Color UI_BG = {15, 23, 42, 255};
static Color UI_PANEL = {30, 41, 59, 255};
static Color UI_CYAN = {34, 211, 238, 255};
static Color UI_GREEN = {74, 222, 128, 255};
static Color UI_RED = {248, 113, 113, 255};
static Color UI_WHITE = {241, 245, 249, 255};

static int ReadValue(int *value)
{
    if (inputLength == 0 ||
        (inputLength == 1 && input[0] == '-'))
        return 0;

    *value = atoi(input);
    return 1;
}

static void ClearInput(void)
{
    inputLength = 0;
    input[0] = '\0';
    inputActive = 0;
}

static void SetMessage(const char *msg)
{
    snprintf(message, sizeof(message), "%s", msg);
}

static void DrawButton(Rectangle r, const char *label, Color color)
{
    Vector2 mouse = GetMousePosition();
    int hover = CheckCollisionPointRec(mouse, r);

    DrawRectangleRounded(r, 0.18f, 8,
                         hover ? ColorBrightness(color, 0.18f) : color);

    DrawText(label,
             (int)(r.x + r.width / 2 - MeasureText(label, 17) / 2),
             (int)(r.y + 13),
             17, UI_WHITE);
}

static int ButtonClicked(Rectangle r)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), r);
}

void DLLVisualizerInit(void)
{
    DLLClear(&dll);
    DLLInit(&dll);

    ClearInput();
    scrollOffset = 0;
    returnDashboard = 0;
    SetMessage("Doubly Linked List is ready.");
}

void DLLVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnDashboard = 1;
        return;
    }

    // Handle horizontal scrolling via mouse wheel or arrow keys
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        scrollOffset -= (int)wheel;
    }
    if (IsKeyPressed(KEY_LEFT) && scrollOffset > 0)
    {
        scrollOffset--;
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        scrollOffset++;
    }

    // Clamp scrollOffset bounds
    int maxScroll = dll.size - NODE_LIMIT;
    if (maxScroll < 0) maxScroll = 0;
    if (scrollOffset < 0) scrollOffset = 0;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Rectangle inputBox = {60, 145, 300, 45};
        inputActive = CheckCollisionPointRec(GetMousePosition(), inputBox);
    }

    if (inputActive)
    {
        int key = GetCharPressed();

        while (key > 0)
        {
            if ((key >= '0' && key <= '9') ||
                (key == '-' && inputLength == 0))
            {
                if (inputLength < 30)
                {
                    input[inputLength++] = (char)key;
                    input[inputLength] = '\0';
                }
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputLength > 0)
        {
            input[--inputLength] = '\0';
        }

        if (IsKeyPressed(KEY_ENTER))
            inputActive = 0;
    }

    int value;

    Rectangle back = {25, 25, 125, 42};
    Rectangle insertFront = {60, 230, 190, 48};
    Rectangle insertEnd = {270, 230, 190, 48};
    Rectangle insertAt = {480, 230, 190, 48};
    Rectangle deleteValue = {690, 230, 190, 48};
    Rectangle search = {900, 230, 190, 48};
    Rectangle reverse = {60, 300, 190, 48};
    Rectangle clear = {270, 300, 190, 48};

    if (ButtonClicked(back))
    {
        returnDashboard = 1;
        return;
    }

    if (ButtonClicked(insertFront))
    {
        if (ReadValue(&value))
        {
            SetMessage(DLLInsertFront(&dll, value)
                       ? "Inserted at the front."
                       : "Memory allocation failed.");
            ClearInput();
            scrollOffset = 0;
        }
        else SetMessage("Enter a valid integer.");
    }

    if (ButtonClicked(insertEnd))
    {
        if (ReadValue(&value))
        {
            SetMessage(DLLInsertEnd(&dll, value)
                       ? "Inserted at the end."
                       : "Memory allocation failed.");
            ClearInput();
            if (dll.size > NODE_LIMIT)
                scrollOffset = dll.size - NODE_LIMIT;
        }
        else SetMessage("Enter a valid integer.");
    }

    if (ButtonClicked(insertAt))
    {
        if (ReadValue(&value))
        {
            int position = dll.size / 2;

            SetMessage(DLLInsertAt(&dll, position, value)
                       ? "Inserted at the middle position."
                       : "Insertion failed.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (ButtonClicked(deleteValue))
    {
        if (ReadValue(&value))
        {
            SetMessage(DLLDeleteValue(&dll, value)
                       ? "First matching node deleted."
                       : "Value not found.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (ButtonClicked(search))
    {
        if (ReadValue(&value))
        {
            int position = DLLSearch(&dll, value);

            if (position >= 0)
            {
                snprintf(message, sizeof(message),
                         "Found %d at position %d.", value, position);
                if (position >= scrollOffset && position < scrollOffset + NODE_LIMIT)
                {
                    // Already visible
                }
                else
                {
                    scrollOffset = position > NODE_LIMIT - 1 ? position - (NODE_LIMIT - 1) : 0;
                }
            }
            else
                SetMessage("Value not found.");
        }
        else SetMessage("Enter a valid integer.");
    }

    if (ButtonClicked(reverse))
    {
        DLLReverse(&dll);
        SetMessage("Linked list reversed.");
    }

    if (ButtonClicked(clear))
    {
        DLLClear(&dll);
        SetMessage("Linked list cleared.");
        ClearInput();
        scrollOffset = 0;
    }
}

int DLLVisualizerWantsDashboard(void)
{
    return returnDashboard;
}

void DLLVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    DrawRectangle(0, 0, SCREEN_W, 90, UI_PANEL);

    DrawButton((Rectangle){25, 25, 125, 42},
               "BACK", UI_RED);

    DrawText("DOUBLY LINKED LIST VISUALIZER",
             180, 30, 28, UI_CYAN);

    DrawText("Enter value:", 60, 115, 18, UI_WHITE);

    DrawRectangleRounded((Rectangle){60, 145, 300, 45},
                         0.15f, 8, UI_PANEL);

    DrawRectangleLinesEx((Rectangle){60, 145, 300, 45},
                         2, inputActive ? UI_CYAN : UI_WHITE);

    DrawText(input, 75, 157, 20, UI_WHITE);

    DrawText("Operations", 60, 200, 20, UI_GREEN);

    DrawButton((Rectangle){60, 230, 190, 48},
               "INSERT FRONT", UI_GREEN);

    DrawButton((Rectangle){270, 230, 190, 48},
               "INSERT END", UI_GREEN);

    DrawButton((Rectangle){480, 230, 190, 48},
               "INSERT MIDDLE", UI_GREEN);

    DrawButton((Rectangle){690, 230, 190, 48},
               "DELETE VALUE", UI_RED);

    DrawButton((Rectangle){900, 230, 190, 48},
               "SEARCH", UI_CYAN);

    DrawButton((Rectangle){60, 300, 190, 48},
               "REVERSE", (Color){124, 58, 237, 255});

    DrawButton((Rectangle){270, 300, 190, 48},
               "CLEAR", UI_RED);

    DrawText("VISUAL REPRESENTATION",
             60, 390, 22, UI_CYAN);

    if (scrollOffset == 0)
        DrawText("NULL <- HEAD", 60, 445, 16, UI_WHITE);
    else
        DrawText("<- [SCROLLED]", 60, 445, 16, UI_GREEN);

    // Traverse to the scrollOffset node
    DNode *current = dll.head;
    for (int i = 0; i < scrollOffset && current != NULL; i++)
    {
        current = current->next;
    }

    int x = 180;
    int count = 0;

    while (current != NULL && count < NODE_LIMIT)
    {
        DrawRectangleRounded(
            (Rectangle){x, 465, 105, 65},
            0.12f, 8, UI_PANEL);

        DrawRectangleLinesEx(
            (Rectangle){x, 465, 105, 65},
            2, UI_CYAN);

        DrawText("PREV", x + 5, 472, 10, UI_GREEN);

        char number[20];
        snprintf(number, sizeof(number), "%d", current->data);

        DrawText(number, x + 39, 491, 20, UI_WHITE);

        DrawText("NEXT", x + 70, 472, 10, UI_GREEN);

        if (current->next != NULL && count < NODE_LIMIT - 1)
        {
            DrawLineEx((Vector2){x + 105, 480},
                       (Vector2){x + 155, 480}, 2, UI_CYAN);

            DrawLineEx((Vector2){x + 155, 510},
                       (Vector2){x + 105, 510}, 2, UI_GREEN);

            DrawTriangle(
                (Vector2){x + 155, 480},
                (Vector2){x + 146, 475},
                (Vector2){x + 146, 485}, UI_CYAN);

            DrawTriangle(
                (Vector2){x + 105, 510},
                (Vector2){x + 114, 505},
                (Vector2){x + 114, 515}, UI_GREEN);
        }

        current = current->next;
        x += 170;
        count++;
    }

    if (dll.size == 0)
        DrawText("EMPTY LIST", 180, 485, 24, UI_RED);
    else if (dll.size > NODE_LIMIT)
        DrawText(TextFormat("Use Mouse Wheel or Arrow Keys to Scroll (%d/%d)", scrollOffset + 1, dll.size - NODE_LIMIT + 1),
                 180, 538, 14, UI_WHITE);

    DrawText("FORWARD: HEAD -> TAIL",
             60, 565, 18, UI_GREEN);

    DrawText("BACKWARD: TAIL -> HEAD",
             60, 595, 18, UI_CYAN);

    DrawText(TextFormat("Total Nodes: %d", dll.size),
             60, 635, 18, UI_WHITE);

    DrawText(message, 480, 635, 18, YELLOW);
}