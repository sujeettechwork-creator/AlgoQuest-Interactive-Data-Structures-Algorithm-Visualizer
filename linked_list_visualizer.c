#include "linked_list_visualizer.h"
#include "../structures/linked_list.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

static LinkedList list;
static char input[16] = "";
static int inputLen = 0;
static int inputActive = 1;
static int returnDashboard = 0;
static int scrollOffset = 0; // Tracks horizontal scrolling for long lists

static const Color UI_BG = {15, 18, 32, 255};
static const Color UI_PANEL = {25, 32, 51, 255};
static const Color UI_CYAN = {0, 210, 220, 255};
static const Color UI_PURPLE = {150, 100, 255, 255};
static const Color UI_WHITE = {240, 245, 255, 255};
static const Color UI_MUTED = {145, 157, 180, 255};
static const Color UI_GREEN = {60, 210, 130, 255};
static const Color UI_RED = {245, 90, 105, 255};

static char status[120] = "Enter a value to begin.";
static Color currentStatusColor = {0, 210, 220, 255};

static int Hit(Rectangle r)
{
    return CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void SetStatus(const char *message, Color color)
{
    snprintf(status, sizeof(status), "%s", message);
    currentStatusColor = color;
}

static void Button(Rectangle r, const char *label, Color color)
{
    DrawRectangleRounded(r, 0.15f, 8, color);

    int size = 15;

    DrawText(label,
             (int)(r.x + (r.width - MeasureText(label, size)) / 2),
             (int)(r.y + (r.height - size) / 2),
             size, UI_WHITE);
}

void LinkedListVisualizerInit(void)
{
    LinkedListClear(&list);
    LinkedListInit(&list);

    input[0] = '\0';
    inputLen = 0;
    returnDashboard = 0;
    scrollOffset = 0;

    SetStatus("Enter a value and choose an operation.", UI_CYAN);
}

void LinkedListVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
        returnDashboard = 1;

    // Moved BACK button to x = 970, y = 20 (safe from FPS counter)
    if (Hit((Rectangle){970, 20, 130, 38}))
        returnDashboard = 1;

    // Handle mouse wheel scrolling for long lists
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        scrollOffset -= (int)(wheel * 2);
        if (scrollOffset < 0)
            scrollOffset = 0;
        // Limit max scroll based on list size
        if (scrollOffset > list.size - 1 && list.size > 0)
            scrollOffset = list.size - 1;
    }

    Rectangle inputBox = {285, 170, 180, 48};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        inputActive = CheckCollisionPointRec(
            GetMousePosition(), inputBox);

    if (inputActive)
    {
        int key = GetCharPressed();

        while (key > 0)
        {
            if (((key >= '0' && key <= '9') ||
                 (key == '-' && inputLen == 0)) &&
                inputLen < 14)
            {
                input[inputLen++] = (char)key;
                input[inputLen] = '\0';
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && inputLen > 0)
            input[--inputLen] = '\0';
    }

    int value = atoi(input);

    if (Hit((Rectangle){485, 170, 140, 48}))
    {
        if (!inputLen)
            SetStatus("Enter a value first.", UI_RED);
        else if (LinkedListInsertFront(&list, value))
        {
            SetStatus("Node inserted at HEAD.", UI_GREEN);
            scrollOffset = 0; // Jump to head on front insert
        }
        else
            SetStatus("Memory allocation failed.", UI_RED);

        input[0] = '\0';
        inputLen = 0;
    }

    if (Hit((Rectangle){635, 170, 140, 48}))
    {
        if (!inputLen)
            SetStatus("Enter a value first.", UI_RED);
        else if (LinkedListInsertEnd(&list, value))
        {
            SetStatus("Node inserted at END.", UI_GREEN);
            // Automatically scroll to view the new end node if list is long
            if (list.size > 6)
                scrollOffset = list.size - 6;
        }
        else
            SetStatus("Memory allocation failed.", UI_RED);

        input[0] = '\0';
        inputLen = 0;
    }

    if (Hit((Rectangle){785, 170, 140, 48}))
    {
        if (!inputLen)
            SetStatus("Enter value to delete.", UI_RED);
        else if (LinkedListDeleteValue(&list, value))
            SetStatus("Node deleted successfully.", UI_GREEN);
        else
            SetStatus("Value not found.", UI_RED);

        input[0] = '\0';
        inputLen = 0;
    }

    if (Hit((Rectangle){935, 170, 115, 48}))
    {
        if (!inputLen)
            SetStatus("Enter value to search.", UI_RED);
        else
        {
            int position = LinkedListSearch(&list, value);

            if (position >= 0)
            {
                char message[120];
                snprintf(message, sizeof(message),
                         "Value found at position %d.", position);
                SetStatus(message, UI_GREEN);
                // Auto-scroll to position if found
                scrollOffset = position > 5 ? position - 5 : 0;
            }
            else
                SetStatus("Value not found.", UI_RED);
        }
    }

    if (Hit((Rectangle){1060, 170, 90, 48}))
    {
        LinkedListClear(&list);
        scrollOffset = 0;
        SetStatus("All nodes cleared and memory freed.", UI_CYAN);
    }

    if (Hit((Rectangle){485, 235, 140, 42}))
    {
        if (!inputLen)
            SetStatus("Enter a value first.", UI_RED);
        else
        {
            int midIndex = list.size / 2;
            if (LinkedListInsertAt(&list, midIndex, value))
                SetStatus("Node inserted at middle position.", UI_GREEN);
            else
                SetStatus("Insertion failed.", UI_RED);
        }

        input[0] = '\0';
        inputLen = 0;
    }
}

void LinkedListVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    // BACK button placed safely away from FPS counter
    Button((Rectangle){970, 20, 130, 38},
           "BACK", (Color){55, 65, 90, 255});

    DrawText("LINKED LIST VISUALIZER",
             285, 30, 28, UI_CYAN);

    DrawText("Dynamic memory allocation and pointer connections",
             285, 68, 15, UI_MUTED);

    DrawRectangleRounded(
        (Rectangle){285, 115, 880, 175},
        0.04f, 8, UI_PANEL);

    DrawText("INPUT VALUE", 300, 135, 11, UI_MUTED);

    DrawRectangleRounded(
        (Rectangle){300, 160, 170, 48},
        0.12f, 8, (Color){32, 41, 64, 255});

    DrawText(inputLen ? input : "Enter value",
             315, 174, 18, inputLen ? UI_WHITE : UI_MUTED);

    Button((Rectangle){485, 160, 140, 48},
           "INSERT FRONT", UI_CYAN);

    Button((Rectangle){635, 160, 140, 48},
           "INSERT END", UI_PURPLE);

    Button((Rectangle){785, 160, 140, 48},
           "DELETE VALUE", UI_RED);

    Button((Rectangle){935, 160, 115, 48},
           "SEARCH", (Color){65, 105, 150, 255});

    Button((Rectangle){1060, 160, 90, 48},
           "CLEAR", UI_RED);

    Button((Rectangle){485, 225, 140, 42},
           "INSERT MIDDLE", (Color){65, 105, 150, 255});

    DrawText("LINKED LIST MEMORY",
             285, 320, 18, UI_WHITE);

    DrawText(TextFormat("SIZE: %d NODES", list.size),
             1015, 320, 16, UI_CYAN);

    Node *current = list.head;
    int currentIndex = 0;

    // Traverse to the scroll offset node
    while (current != NULL && currentIndex < scrollOffset)
    {
        current = current->next;
        currentIndex++;
    }

    int x = 295;
    int y = 410;
    int drawnCount = 0;

    if (list.head == NULL)
    {
        DrawText("HEAD -> NULL (EMPTY LIST)",
                 300, 415, 20, UI_MUTED);
    }
    else if (current == NULL)
    {
        DrawText("End of list reached.", 300, 415, 18, UI_MUTED);
    }

    while (current != NULL && drawnCount < 6)
    {
        Rectangle node = {(float)x, (float)y, 105, 70};

        // Highlight head differently if it's visible, otherwise show actual index
        int absoluteIndex = currentIndex + drawnCount;
        DrawRectangleRounded(node, 0.1f, 7,
                             absoluteIndex == 0 ? UI_PURPLE : (Color){32, 41, 64, 255});

        DrawRectangleLinesEx(node, 2, UI_CYAN);

        DrawText("DATA", x + 8, y + 7, 10, UI_MUTED);

        DrawText(TextFormat("%d", current->data),
                 x + 12, y + 29, 22, UI_WHITE);

        DrawText(absoluteIndex == 0 ? "HEAD" : TextFormat("NODE %d", absoluteIndex),
                 x + 8, y + 78, 11, UI_CYAN);

        if (current->next != NULL)
        {
            DrawLineEx(
                (Vector2){x + 106, y + 35},
                (Vector2){x + 137, y + 35},
                3, UI_GREEN);

            DrawTriangle(
                (Vector2){x + 137, y + 29},
                (Vector2){x + 147, y + 35},
                (Vector2){x + 137, y + 41},
                UI_GREEN);
        }
        else
        {
            DrawText("NULL", x + 112, y + 27, 15, UI_RED);
        }

        current = current->next;
        x += 155;
        drawnCount++;
    }

    // Scroll helper instruction indicator
    if (list.size > 6)
    {
        DrawText(TextFormat("Use Mouse Wheel to Scroll (Showing nodes %d - %d of %d)", 
                 scrollOffset, scrollOffset + drawnCount - 1, list.size),
                 285, 525, 13, UI_MUTED);
    }

    DrawRectangleRounded(
        (Rectangle){285, 575, 880, 105},
        0.05f, 8, UI_PANEL);

    DrawText("STATUS", 305, 590, 12, UI_MUTED);

    DrawText(status, 305, 615, 17, currentStatusColor);

    DrawText("Insert/Delete at HEAD: O(1) | Search: O(n)",
             305, 647, 13, UI_MUTED);
}

int LinkedListVisualizerWantsDashboard(void)
{
    if (returnDashboard)
    {
        returnDashboard = 0;
        return 1;
    }

    return 0;
}