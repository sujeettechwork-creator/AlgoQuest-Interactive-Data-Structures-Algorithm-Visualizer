#include "binary_tree_visualizer.h"
#include "../structures/binary_tree.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1200
#define HEIGHT 700
#define MAX_NODES 100

static BinarySearchTree tree;
static char input[32] = "";
static int inputLength = 0;
static int inputActive = 0;
static int returnDashboard = 0;
static int highlightedValue = 0;
static int highlightActive = 0;
static char message[200] = "Binary Search Tree Ready. (Scroll to Zoom, Right-Click Drag to Pan)";

// Camera for panning and zooming around the tree canvas
static Camera2D bstCam = {
    .offset = {0.0f, 0.0f},
    .target = {0.0f, 0.0f},
    .rotation = 0.0f,
    .zoom = 1.0f};
static int isDragging = 0;
static Vector2 dragStartPos = {0};

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
                   MeasureText(label, 16) / 2),
             (int)(r.y + 13), 16, UI_WHITE);
}

static void DrawTreeNode(TreeNode *node, float x,
                         float y, float offset, int depth)
{
    if (node == NULL || depth > 20)
        return;

    float nextY = y + 65;
    float nextOffset = offset * 0.65f;

    if (node->left != NULL)
    {
        DrawLineEx(
            (Vector2){x, y},
            (Vector2){x - offset, nextY},
            2, UI_GREEN);

        DrawTreeNode(node->left,
                     x - offset,
                     nextY,
                     nextOffset,
                     depth + 1);
    }

    if (node->right != NULL)
    {
        DrawLineEx(
            (Vector2){x, y},
            (Vector2){x + offset, nextY},
            2, UI_CYAN);

        DrawTreeNode(node->right,
                     x + offset,
                     nextY,
                     nextOffset,
                     depth + 1);
    }

    Color nodeColor = UI_PANEL;

    if (highlightActive && node->data == highlightedValue)
        nodeColor = UI_GREEN;

    DrawCircle((int)x, (int)y, 20, nodeColor);
    DrawCircleLines((int)x, (int)y, 20, UI_CYAN);

    DrawText(TextFormat("%d", node->data),
             (int)x - MeasureText(TextFormat("%d", node->data), 14) / 2,
             (int)y - 7, 14, UI_WHITE);
}

static void ShowTraversal(int type)
{
    int output[MAX_NODES];
    int count = 0;

    if (type == 0)
        count = BSTInorder(&tree, output, MAX_NODES);
    else if (type == 1)
        count = BSTPreorder(&tree, output, MAX_NODES);
    else
        count = BSTPostorder(&tree, output, MAX_NODES);

    const char *name = type == 0 ? "INORDER" : type == 1 ? "PREORDER"
                                                         : "POSTORDER";

    int position = snprintf(message, sizeof(message),
                            "%s: ", name);

    for (int i = 0; i < count && position < 170; i++)
    {
        position += snprintf(message + position,
                             sizeof(message) - position,
                             "%d ", output[i]);
    }

    if (count == 0)
        SetMessage("Tree is empty.");
}

void BSTVisualizerInit(void)
{
    BSTClear(&tree);
    BSTInit(&tree);

    ClearInput();
    returnDashboard = 0;
    highlightActive = 0;

    bstCam.zoom = 1.0f;
    bstCam.target = (Vector2){0, 0};
    bstCam.offset = (Vector2){0, 0};

    SetMessage("Binary Search Tree Ready. (Scroll to Zoom, Right-Click Drag to Pan)");
}

void BSTVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        returnDashboard = 1;
        return;
    }

    // --- Camera Zoom (Mouse Wheel) ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.y > 250)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(mousePos, bstCam);
            bstCam.offset = mousePos;
            bstCam.target = mouseWorldPos;

            bstCam.zoom += wheel * 0.1f;
            if (bstCam.zoom < 0.2f)
                bstCam.zoom = 0.2f;
            if (bstCam.zoom > 3.0f)
                bstCam.zoom = 3.0f;
        }
    }

    // --- Camera Pan (Right Mouse Button Drag) ---
    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && mousePos.y > 250)
    {
        isDragging = 1;
        dragStartPos = GetMousePosition();
    }
    if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 currentMouse = GetMousePosition();
        Vector2 delta = {(currentMouse.x - dragStartPos.x) / bstCam.zoom, (currentMouse.y - dragStartPos.y) / bstCam.zoom};
        bstCam.target.x -= delta.x;
        bstCam.target.y -= delta.y;
        dragStartPos = currentMouse;
    }
    else
    {
        isDragging = 0;
    }

    Rectangle inputBox = {60, 135, 250, 42};

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

    Rectangle back = {25, 20, 120, 40};
    Rectangle insert = {340, 135, 130, 42};
    Rectangle search = {485, 135, 130, 42};
    Rectangle deleteNode = {630, 135, 130, 42};
    Rectangle minimum = {775, 135, 130, 42};
    Rectangle maximum = {920, 135, 130, 42};
    Rectangle clear = {1060, 135, 110, 42};

    Rectangle inorder = {60, 195, 150, 42};
    Rectangle preorder = {230, 195, 150, 42};
    Rectangle postorder = {400, 195, 150, 42};

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
            int result = BSTInsert(&tree, value);
            highlightActive = 0;

            if (result)
                SetMessage("Node inserted successfully.");
            else
                SetMessage("Duplicate value or memory allocation failure.");

            ClearInput();
        }
        else
            SetMessage("Enter a valid integer.");
    }

    if (Clicked(search))
    {
        if (ReadValue(&value))
        {
            highlightActive = BSTSearch(&tree, value);
            highlightedValue = value;

            SetMessage(highlightActive
                           ? "Value found and highlighted."
                           : "Value not found.");
        }
        else
            SetMessage("Enter a valid integer.");
    }

    if (Clicked(deleteNode))
    {
        if (ReadValue(&value))
        {
            highlightActive = 0;

            SetMessage(BSTDelete(&tree, value)
                           ? "Node deleted successfully."
                           : "Value not found.");

            ClearInput();
        }
        else
            SetMessage("Enter a valid integer.");
    }

    if (Clicked(minimum))
    {
        if (BSTMinimum(&tree, &value))
            snprintf(message, sizeof(message),
                     "Minimum value: %d", value);
        else
            SetMessage("Tree is empty.");
    }

    if (Clicked(maximum))
    {
        if (BSTMaximum(&tree, &value))
            snprintf(message, sizeof(message),
                     "Maximum value: %d", value);
        else
            SetMessage("Tree is empty.");
    }

    if (Clicked(clear))
    {
        BSTClear(&tree);
        highlightActive = 0;
        SetMessage("Tree cleared.");
        ClearInput();
    }

    if (Clicked(inorder))
        ShowTraversal(0);

    if (Clicked(preorder))
        ShowTraversal(1);

    if (Clicked(postorder))
        ShowTraversal(2);
}

int BSTVisualizerWantsDashboard(void)
{
    return returnDashboard;
}

void BSTVisualizerDraw(void)
{
    ClearBackground(UI_BG);

    // --- UI Elements (Fixed on Screen) ---
    DrawRectangle(0, 0, WIDTH, 85, UI_PANEL);

    DrawButton((Rectangle){25, 20, 120, 40},
               "BACK", UI_RED);

    DrawText("BINARY SEARCH TREE VISUALIZER",
             175, 25, 27, UI_CYAN);

    DrawText("Enter Value:", 60, 105, 17, UI_WHITE);

    DrawRectangleRounded((Rectangle){60, 135, 250, 42},
                         0.15f, 8, UI_PANEL);

    DrawRectangleLinesEx((Rectangle){60, 135, 250, 42},
                         2, inputActive ? UI_CYAN : UI_WHITE);

    DrawText(input, 75, 147, 19, UI_WHITE);

    DrawButton((Rectangle){340, 135, 130, 42},
               "INSERT", UI_GREEN);

    DrawButton((Rectangle){485, 135, 130, 42},
               "SEARCH", UI_CYAN);

    DrawButton((Rectangle){630, 135, 130, 42},
               "DELETE", UI_RED);

    DrawButton((Rectangle){775, 135, 130, 42},
               "MINIMUM", UI_GREEN);

    DrawButton((Rectangle){920, 135, 130, 42},
               "MAXIMUM", UI_GREEN);

    DrawButton((Rectangle){1060, 135, 110, 42},
               "CLEAR", UI_RED);

    DrawButton((Rectangle){60, 195, 150, 42},
               "INORDER", UI_CYAN);

    DrawButton((Rectangle){230, 195, 150, 42},
               "PREORDER", UI_GREEN);

    DrawButton((Rectangle){400, 195, 150, 42},
               "POSTORDER", (Color){124, 58, 237, 255});

    DrawText("TREE VISUALIZATION (Right-Click Drag to Pan, Scroll to Zoom)",
             60, 250, 14, UI_CYAN);

    // --- Tree Canvas (Pannable & Zoomable via Camera) ---
    BeginMode2D(bstCam);

    if (tree.root == NULL)
    {
        DrawText("EMPTY TREE",
                 500, 430, 28, UI_RED);
    }
    else
    {
        DrawTreeNode(tree.root, 475, 290, 180, 0);
    }

    EndMode2D();

    // --- Bottom Status Bar (Fixed on Screen) ---
    DrawRectangle(0, 620, WIDTH, 80, UI_PANEL);
    DrawText(TextFormat("Total Nodes: %d", tree.size),
             60, 645, 18, UI_WHITE);

    DrawText(message, 250, 645, 15, UI_GREEN);
}