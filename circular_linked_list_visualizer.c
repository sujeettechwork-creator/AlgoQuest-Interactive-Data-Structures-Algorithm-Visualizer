#include "circular_linked_list_visualizer.h"
#include "../structures/circular_linked_list.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 700
#define DISPLAY_LIMIT 6

static CircularLinkedList list;
static char input[32] = "";
static int inputLength = 0;
static int inputActive = 0;
static int returnDashboard = 0;
static char message[100] = "Circular Linked List Ready";

static Color UI_BG = {15, 23, 42, 255};
static Color UI_PANEL = {30, 41, 59, 255};
static Color UI_CYAN = {34, 211, 238, 255};
static Color UI_GREEN = {74, 222, 128, 255};
static Color UI_RED = {248, 113, 113, 255};
static Color UI_WHITE = {241, 245, 249, 255};

static void SetMessage(const char *msg)
{
    snprintf(message, sizeof(message), "%s", msg);
}

static void ClearInput(void)
{
    inputLength = 0;
    input[0] = '\0';
    inputActive = 0;
}

static int ReadValue(int *value)
{
    if (inputLength == 0 ||
        (inputLength == 1 && input[0] == '-'))
        return 0;

    *value = atoi(input);
    return 1;
}

static int Clicked(Rectangle r)
{
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), r);
}

static void DrawButton(Rectangle r, const char *label, Color color)
{
    int hover = CheckCollisionPointRec(GetMousePosition(), r);

    DrawRectangleRounded(
        r, 0.15f, 8,
        hover ? ColorBrightness(color, 0.18f) : color);

    DrawText(label,
             (int)(r.x + r.width / 2 -
                   MeasureText(label, 17) / 2),
             (int)(r.y + 14), 17, UI_WHITE);
}

void CLLVisualizerInit(void)
{
    CLLClear(&list);
    CLLInit(&list);

    ClearInput();
    returnDashboard = 0;
    SetMessage("Circular Linked List Ready.");
}

void CLLVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnDashboard = 1;
        return;
    }

    Rectangle inputBox = {60, 145, 300, 45};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        inputActive = CheckCollisionPointRec(
            GetMousePosition(), inputBox);

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
            input[--inputLength] = '\0';

        if (IsKeyPressed(KEY_ENTER))
            inputActive = 0;
    }

    Rectangle back = {25, 25, 125, 42};
    Rectangle front = {60, 230, 190, 48};
    Rectangle end = {270, 230, 190, 48};
    Rectangle middle = {480, 230, 190, 48};
    Rectangle del = {690, 230, 190, 48};
    Rectangle search = {900, 230, 190, 48};
    Rectangle reverse = {60, 300, 190, 48};
    Rectangle clear = {270, 300, 190, 48};

    if (Clicked(back))
    {
        returnDashboard = 1;
        return;
    }

    int value;

    if (Clicked(front))
    {
        if (ReadValue(&value))
        {
            SetMessage(CLLInsertFront(&list, value)
                       ? "Inserted at front."
                       : "Memory allocation failed.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(end))
    {
        if (ReadValue(&value))
        {
            SetMessage(CLLInsertEnd(&list, value)
                       ? "Inserted at end."
                       : "Memory allocation failed.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(middle))
    {
        if (ReadValue(&value))
        {
            SetMessage(CLLInsertAt(&list, list.size / 2, value)
                       ? "Inserted at middle."
                       : "Insertion failed.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(del))
    {
        if (ReadValue(&value))
        {
            SetMessage(CLLDeleteValue(&list, value)
                       ? "First matching node deleted."
                       : "Value not found.");
            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(search))
    {
        if (ReadValue(&value))
        {
            int position = CLLSearch(&list, value);

            if (position >= 0)
                snprintf(message, sizeof(message),
                         "Found %d at position %d.", value, position);
            else
                SetMessage("Value not found.");
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(reverse))
    {
        CLLReverse(&list);
        SetMessage("Circular list reversed.");
    }

    if (Clicked(clear))
    {
        CLLClear(&list);
        SetMessage("Circular list cleaUI_RED.");
        ClearInput();
    }
}

int CLLVisualizerWantsDashboard(void)
{
    return returnDashboard;
}

void CLLVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    DrawRectangle(0, 0, WIDTH, 90, UI_PANEL);

    DrawButton((Rectangle){25, 25, 125, 42},
               "BACK", UI_RED);

    DrawText("CIRCULAR LINKED LIST VISUALIZER",
             180, 30, 27, UI_CYAN);

    DrawText("Enter Value:", 60, 115, 18, UI_WHITE);

    DrawRectangleRounded((Rectangle){60, 145, 300, 45},
                         0.15f, 8, UI_PANEL);

    DrawRectangleLinesEx((Rectangle){60, 145, 300, 45},
                         2, inputActive ? UI_CYAN : UI_WHITE);

    DrawText(input, 75, 157, 20, UI_WHITE);

    DrawText("OPERATIONS", 60, 200, 20, UI_GREEN);

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

    DrawText("CIRCULAR STRUCTURE",
             60, 390, 22, UI_CYAN);

    if (list.size == 0)
    {
        DrawText("EMPTY LIST", 180, 485, 24, UI_RED);
    }
    else
    {
        CNode *current = list.tail->next;
        int x = 180;
        int count = 0;

        while (count < list.size && count < DISPLAY_LIMIT)
        {
            DrawRectangleRounded(
                (Rectangle){x, 465, 105, 65},
                0.12f, 8, UI_PANEL);

            DrawRectangleLinesEx(
                (Rectangle){x, 465, 105, 65},
                2, UI_CYAN);

            char number[20];
            snprintf(number, sizeof(number), "%d",
                     current->data);

            DrawText(number, x + 38, 486, 22, UI_WHITE);

            if (count < DISPLAY_LIMIT - 1 &&
                count < list.size - 1)
            {
                DrawLineEx(
                    (Vector2){x + 105, 497},
                    (Vector2){x + 165, 497},
                    2, UI_GREEN);

                DrawTriangle(
                    (Vector2){x + 165, 497},
                    (Vector2){x + 155, 492},
                    (Vector2){x + 155, 502},
                    UI_GREEN);
            }

            current = current->next;
            x += 170;
            count++;
        }

        DrawText("TAIL -> HEAD (CIRCULAR LINK)",
                 180, 555, 18, UI_GREEN);

        if (list.size > DISPLAY_LIMIT)
            DrawText("More nodes exist than currently displayed.",
                     180, 580, 15, UI_WHITE);
    }

    DrawText(TextFormat("Total Nodes: %d", list.size),
             60, 625, 18, UI_WHITE);

    DrawText(message, 400, 625, 18, YELLOW);
}