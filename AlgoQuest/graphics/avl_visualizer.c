#include "avl_visualizer.h"
#include "../structures/avl_tree.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1200
#define MAX_NODES 100

static AVLTree tree;
static char input[32] = "";
static int inputLength = 0;
static int inputActive = 0;
static int returnDashboard = 0;
static int highlightedValue = 0;
static int highlightActive = 0;
static char message[180] = "AVL Tree Ready.";

static Color UI_BG = {15, 23, 42, 255};
static Color UI_PANEL = {30, 41, 59, 255};
static Color UI_CYAN = {34, 211, 238, 255};
static Color UI_GREEN = {74, 222, 128, 255};
static Color UI_RED = {248, 113, 113, 255};
static Color UI_WHITE = {241, 245, 249, 255};

static int Height(AVLNode *node)
{
    return node == NULL ? 0 : node->height;
}

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
                   MeasureText(label, 15) / 2),
             (int)(r.y + 13), 15, UI_WHITE);
}

static void DrawAVLNode(AVLNode *node, float x, float y,
                        float offset, int depth)
{
    if (node == NULL || depth > 6)
        return;

    float nextY = y + 75;
    float nextOffset = offset / 2.0f;

    if (node->left != NULL)
    {
        DrawLineEx((Vector2){x, y},
                   (Vector2){x - offset, nextY},
                   3, UI_GREEN);

        DrawAVLNode(node->left, x - offset,
                    nextY, nextOffset, depth + 1);
    }

    if (node->right != NULL)
    {
        DrawLineEx((Vector2){x, y},
                   (Vector2){x + offset, nextY},
                   3, UI_CYAN);

        DrawAVLNode(node->right, x + offset,
                    nextY, nextOffset, depth + 1);
    }

    int balance = Height(node->left) - Height(node->right);

    Color nodeColor = UI_PANEL;

    if (highlightActive && node->data == highlightedValue)
        nodeColor = UI_GREEN;

    DrawCircle((int)x, (int)y, 25, nodeColor);
    DrawCircleLines((int)x, (int)y, 25, UI_CYAN);

    DrawText(TextFormat("%d", node->data),
             (int)x - MeasureText(TextFormat("%d", node->data), 17) / 2,
             (int)y - 12, 17, UI_WHITE);

    DrawText(TextFormat("H:%d B:%d", node->height, balance),
             (int)x - 37, (int)y + 30, 12, UI_WHITE);
}

static void ShowTraversal(int type)
{
    int output[MAX_NODES];
    int count;

    if (type == 0)
        count = AVLInorder(&tree, output, MAX_NODES);
    else if (type == 1)
        count = AVLPreorder(&tree, output, MAX_NODES);
    else
        count = AVLPostorder(&tree, output, MAX_NODES);

    const char *name = type == 0 ? "INORDER" :
                       type == 1 ? "PREORDER" : "POSTORDER";

    int position = snprintf(message, sizeof(message), "%s: ", name);

    for (int i = 0; i < count && position < 160; i++)
    {
        position += snprintf(message + position,
                             sizeof(message) - position,
                             "%d ", output[i]);
    }

    if (count == 0)
        SetMessage("Tree is empty.");
}

void AVLVisualizerInit(void)
{
    AVLClear(&tree);
    AVLInit(&tree);

    ClearInput();
    returnDashboard = 0;
    highlightActive = 0;

    SetMessage("AVL Tree Ready.");
}

void AVLVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnDashboard = 1;
        return;
    }

    Rectangle inputBox = {50, 125, 220, 42};

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

    Rectangle back = {20, 20, 110, 38};
    Rectangle insert = {290, 125, 110, 42};
    Rectangle search = {410, 125, 110, 42};
    Rectangle deleteNode = {530, 125, 110, 42};
    Rectangle clear = {650, 125, 110, 42};

    Rectangle inorder = {50, 185, 140, 40};
    Rectangle preorder = {205, 185, 140, 40};
    Rectangle postorder = {360, 185, 140, 40};

    if (Clicked(back))
    {
        returnDashboard = 1;
        return;
    }

    int value;

    if (Clicked(insert))
    {
        if (ReadValue(&value))
        {
            int result = AVLInsert(&tree, value);
            highlightActive = 0;

            SetMessage(result
                       ? "Inserted. AVL balancing applied."
                       : "Duplicate value or memory allocation failure.");

            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(search))
    {
        if (ReadValue(&value))
        {
            highlightActive = AVLSearch(&tree, value);
            highlightedValue = value;

            SetMessage(highlightActive
                       ? "Node found and highlighted."
                       : "Node not found.");
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(deleteNode))
    {
        if (ReadValue(&value))
        {
            highlightActive = 0;

            SetMessage(AVLDelete(&tree, value)
                       ? "Deleted. AVL balancing applied."
                       : "Value not found.");

            ClearInput();
        }
        else SetMessage("Enter a valid integer.");
    }

    if (Clicked(clear))
    {
        AVLClear(&tree);
        highlightActive = 0;
        ClearInput();
        SetMessage("AVL Tree cleared.");
    }

    if (Clicked(inorder))
        ShowTraversal(0);

    if (Clicked(preorder))
        ShowTraversal(1);

    if (Clicked(postorder))
        ShowTraversal(2);
}

int AVLVisualizerWantsDashboard(void)
{
    return returnDashboard;
}

void AVLVisualizerDraw(void)
{
    ClearBackground(UI_BG);
    DrawRectangle(0, 0, WIDTH, 85, UI_PANEL);

    DrawButton((Rectangle){20, 20, 110, 38},
               "BACK", UI_RED);

    DrawText("AVL TREE VISUALIZER",
             160, 25, 28, UI_CYAN);

    DrawText("Enter Value:", 50, 95, 17, UI_WHITE);

    DrawRectangleRounded((Rectangle){50, 125, 220, 42},
                         0.15f, 8, UI_PANEL);

    DrawRectangleLinesEx((Rectangle){50, 125, 220, 42},
                         2, inputActive ? UI_CYAN : UI_WHITE);

    DrawText(input, 65, 137, 19, UI_WHITE);

    DrawButton((Rectangle){290, 125, 110, 42},
               "INSERT", UI_GREEN);

    DrawButton((Rectangle){410, 125, 110, 42},
               "SEARCH", UI_CYAN);

    DrawButton((Rectangle){530, 125, 110, 42},
               "DELETE", UI_RED);

    DrawButton((Rectangle){650, 125, 110, 42},
               "CLEAR", UI_RED);

    DrawButton((Rectangle){50, 185, 140, 40},
               "INORDER", UI_CYAN);

    DrawButton((Rectangle){205, 185, 140, 40},
               "PREORDER", UI_GREEN);

    DrawButton((Rectangle){360, 185, 140, 40},
               "POSTORDER", (Color){124, 58, 237, 255});

    DrawText("AVL TREE STRUCTURE",
             50, 255, 22, UI_CYAN);

    if (tree.root == NULL)
        DrawText("EMPTY TREE", 500, 430, 28, UI_RED);
    else
        DrawAVLNode(tree.root, 600, 325, 230, 0);

    DrawText(TextFormat("Total Nodes: %d", tree.size),
             50, 645, 18, UI_WHITE);

    DrawText(message, 280, 645, 16, UI_GREEN);
}