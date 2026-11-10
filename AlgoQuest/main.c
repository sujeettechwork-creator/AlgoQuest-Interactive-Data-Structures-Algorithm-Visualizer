#include "raylib.h"
#include "graphics/stack_visualizer.h"
#include "graphics/queue_visualizer.h"
#include "graphics/linked_list_visualizer.h"
#include "graphics/doubly_linked_list_visualizer.h"
#include "graphics/circular_linked_list_visualizer.h"
#include "graphics/binary_tree_visualizer.h"
#include "graphics/avl_visualizer.h"
#include "graphics/heap_visualizer.h"
#include "graphics/graph_visualizer.h"
#include "graphics/sorting_visualizer.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750
#define SIDEBAR_WIDTH 250

// Scale used for visualizers that were designed for the full 1200px width
#define FIT_SCALE ((float)(SCREEN_WIDTH - SIDEBAR_WIDTH) / (float)SCREEN_WIDTH)

static const Color UI_BACKGROUND = {15, 18, 32, 255};
static const Color UI_SIDEBAR = {21, 26, 43, 255};
static const Color UI_CARD = {29, 36, 56, 255};
static const Color UI_CYAN = {0, 210, 220, 255};
static const Color UI_PURPLE = {150, 100, 255, 255};
static const Color UI_WHITE = {240, 245, 255, 255};
static const Color UI_MUTED = {145, 157, 180, 255};

static int selectedModule = 0;

// 0: Dashboard
// 1: Sorting Visualizer
// 2: Stack Visualizer
// 3: Queue Visualizer
// 4: Linked List Visualizer
// 5: Doubly Linked List Visualizer
// 6: Circular Linked List
// 7: Binary Search Tree Visualizer
// 8: AVL Tree Visualizer
// 9: Heap & Priority Queue
// 10: Graph Algorithms
// 11: AlgoQuest Challenge (Placeholder)
static const char *modules[] = {
    "Dashboard",
    "Sorting Visualizer",
    "Stack Visualizer",
    "Queue Visualizer",
    "Linked List Visualizer",
    "Doubly Linked List Visualizer",
    "Circular Linked List",
    "Binary Search Tree Visualizer",
    "AVL Tree Visualizer",
    "Heap & Priority Queue",
    "Graph Algorithms",
    "AlgoQuest Challenge"};
#define MODULE_COUNT 12

// 1 = visualizer was made for full 1200px width -> scale it to fit beside the sidebar
// 0 = draw normally (already leaves room for the sidebar)
static const int moduleNeedsFit[MODULE_COUNT] = {
    0, // Dashboard
    0, // Sorting
    1, // Stack
    1, // Queue
    0, // Linked List
    1, // Doubly Linked List
    1, // Circular Linked List
    1, // Binary Search Tree Visualizer
    1, // AVL Tree Visualizer
    1, // Heap & Priority Queue (Fit enabled)
    0, // Graph Algorithms
    0  // AlgoQuest Challenge
};

static void ResetMouseMapping(void)
{
    SetMouseOffset(0, 0);
    SetMouseScale(1.0f, 1.0f);
}

static void ApplyFitMouseMapping(void)
{
    SetMouseOffset(-SIDEBAR_WIDTH, 0);
    SetMouseScale(1.0f / FIT_SCALE, 1.0f / FIT_SCALE);
}

static void DrawSidebar(void)
{
    DrawRectangle(0, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, UI_SIDEBAR);
    DrawText("ALGO", 30, 20, 28, UI_CYAN);
    DrawText("QUEST", 105, 20, 28, UI_WHITE);
    DrawText("LEARN DATA STRUCTURES", 30, 55, 10, UI_MUTED);

    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < MODULE_COUNT; i++)
    {
        Rectangle button = {20, 75 + i * 36, 210, 32};
        int hover = CheckCollisionPointRec(mousePos, button);
        Color fill = selectedModule == i ? UI_CYAN : (hover ? (Color){45, 55, 80, 255} : UI_CARD);
        DrawRectangleRounded(button, 0.18f, 8, fill);
        DrawText(modules[i], (int)button.x + 10, (int)button.y + 9, 11,
                 selectedModule == i ? UI_BACKGROUND : UI_WHITE);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedModule = i;
        }
    }

    DrawLine(20, 515, 230, 515, UI_MUTED);
    DrawText("ALGOQUEST v1.2", 30, 528, 13, UI_CYAN);
    DrawText("Developed in C", 30, 548, 11, UI_MUTED);
}

static void DrawDashboard(void)
{
    DrawText("Welcome to AlgoQuest", 285, 35, 30, UI_WHITE);
    DrawText("Explore. Visualize. Master Algorithms.", 285, 80, 16, UI_MUTED);

    const char *titles[] = {"SORTING", "STACK", "QUEUE",
                          "LINKED LIST", "DOUBLY LINKED", "CIRCULAR LIST"};
    const char *descriptions[] = {"Visualize sorting algorithms",
                                 "Practice PUSH, POP and PEEK", "Explore ENQUEUE and DEQUEUE",
                                 "Understand node connections", "Explore bidirectional links",
                                 "Explore circular node loops"};
    Color accents[] = {UI_CYAN, UI_PURPLE, UI_CYAN, UI_PURPLE, UI_CYAN, UI_PURPLE};

    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < 6; i++)
    {
        int col = i % 3;
        int row = i / 3;
        Rectangle card = {285 + col * 290, 145 + row * 215, 265, 180};
        int hover = CheckCollisionPointRec(mousePos, card);
        DrawRectangleRounded(card, 0.12f, 8,
                            hover ? (Color){39, 48, 72, 255} : UI_CARD);
        DrawRectangle((int)card.x, (int)card.y, 265, 5, accents[i]);
        DrawText(titles[i], (int)card.x + 18, (int)card.y + 30, 18, accents[i]);
        DrawText(descriptions[i], (int)card.x + 18, (int)card.y + 75, 13, UI_WHITE);
        DrawText("OPEN MODULE", (int)card.x + 18, (int)card.y + 135, 12, UI_MUTED);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            selectedModule = i + 1;
        }
    }

    DrawText("Phase 2: Data Structures Visualizer", 285, 600, 19, UI_WHITE);
    DrawText("Select a module from the sidebar or dashboard cards.",
             285, 635, 15, UI_MUTED);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AlgoQuest - Data Structures Visualizer");
    SetTargetFPS(60);

    SetExitKey(KEY_NULL);

    StackVisualizerInit();
    QueueVisualizerInit();
    LinkedListVisualizerInit();
    DLLVisualizerInit();
    CLLVisualizerInit();
    BSTVisualizerInit();
    AVLVisualizerInit();
    HeapVisualizerInit();
    GraphVisualizerInit();
    SortingVisualizerInit();

    while (!WindowShouldClose())
    {
        if (moduleNeedsFit[selectedModule])
            ApplyFitMouseMapping();
        else
            ResetMouseMapping();

        if (selectedModule == 1)
        {
            SortingVisualizerUpdate();
            if (SortingVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 2)
        {
            StackVisualizerUpdate();
            if (StackVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 3)
        {
            QueueVisualizerUpdate();
            if (QueueVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 4)
        {
            LinkedListVisualizerUpdate();
            if (LinkedListVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 5)
        {
            DLLVisualizerUpdate();
            if (DLLVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 6)
        {
            CLLVisualizerUpdate();
            if (CLLVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 7)
        {
            BSTVisualizerUpdate();
            if (BSTVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 8)
        {
            AVLVisualizerUpdate();
            if (AVLVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 9)
        {
            HeapVisualizerUpdate();
            if (HeapVisualizerWantsDashboard())
                selectedModule = 0;
        }
        else if (selectedModule == 10)
        {
            GraphVisualizerUpdate();
            if (GraphVisualizerWantsDashboard())
                selectedModule = 0;
        }

        BeginDrawing();
        ClearBackground(UI_BACKGROUND);

        BeginScissorMode(SIDEBAR_WIDTH, 0, SCREEN_WIDTH - SIDEBAR_WIDTH, SCREEN_HEIGHT);

        Camera2D cam = {0};
        cam.offset = (Vector2){0.0f, 0.0f};
        cam.target = (Vector2){0.0f, 0.0f};
        cam.zoom = 1.0f;

        if (moduleNeedsFit[selectedModule])
        {
            cam.offset = (Vector2){(float)SIDEBAR_WIDTH, 0.0f};
            cam.zoom = FIT_SCALE;
        }

        BeginMode2D(cam);
        if (selectedModule == 0)
        {
            DrawDashboard();
        }
        else if (selectedModule == 1)
        {
            SortingVisualizerDraw();
        }
        else if (selectedModule == 2)
        {
            StackVisualizerDraw();
        }
        else if (selectedModule == 3)
        {
            QueueVisualizerDraw();
        }
        else if (selectedModule == 4)
        {
            LinkedListVisualizerDraw();
        }
        else if (selectedModule == 5)
        {
            DLLVisualizerDraw();
        }
        else if (selectedModule == 6)
        {
            CLLVisualizerDraw();
        }
        else if (selectedModule == 7)
        {
            BSTVisualizerDraw();
        }
        else if (selectedModule == 8)
        {
            AVLVisualizerDraw();
        }
        else if (selectedModule == 9)
        {
            HeapVisualizerDraw();
        }
        else if (selectedModule == 10)
        {
            GraphVisualizerDraw();
        }
        else
        {
            DrawText(modules[selectedModule], 285, 45, 30, UI_CYAN);
            DrawText("This module will be implemented in a later phase.",
                     285, 110, 18, UI_WHITE);
        }
        EndMode2D();
        EndScissorMode();

        ResetMouseMapping();
        DrawSidebar();

        DrawFPS(1120, 15);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}