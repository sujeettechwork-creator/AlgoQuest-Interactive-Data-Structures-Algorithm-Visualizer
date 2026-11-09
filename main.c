#include "raylib.h"
#include "graphics/stack_visualizer.h"
#include "graphics/queue_visualizer.h"
#include "graphics/linked_list_visualizer.h"
#include "graphics/doubly_linked_list_visualizer.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750

static const Color UI_BACKGROUND = {15, 18, 32, 255};
static const Color UI_SIDEBAR = {21, 26, 43, 255};
static const Color UI_CARD = {29, 36, 56, 255};
static const Color UI_CYAN = {0, 210, 220, 255};
static const Color UI_PURPLE = {150, 100, 255, 255};
static const Color UI_WHITE = {240, 245, 255, 255};
static const Color UI_MUTED = {145, 157, 180, 255};

static int selectedModule = 0;

static const char *modules[] = {
    "Dashboard",
    "Sorting Visualizer",
    "Stack Visualizer",
    "Queue Visualizer",
    "Linked List Visualizer",
    "Doubly Linked List Visualizer",
    "Tree Visualizer",
    "Graph Algorithms",
    "AlgoQuest Challenge"
};

static void DrawSidebar(void)
{
    DrawRectangle(0, 0, 250, SCREEN_HEIGHT, UI_SIDEBAR);
    DrawText("ALGO", 30, 20, 28, UI_CYAN);
    DrawText("QUEST", 105, 20, 28, UI_WHITE);
    DrawText("LEARN DATA STRUCTURES", 30, 55, 10, UI_MUTED);

    for (int i = 0; i < 9; i++)
    {
        Rectangle button = {20, 75 + i * 50, 210, 40};
        int hover = CheckCollisionPointRec(GetMousePosition(), button);
        Color fill = selectedModule == i ? UI_CYAN : (hover ? (Color){45, 55, 80, 255} : UI_CARD);
        DrawRectangleRounded(button, 0.18f, 8, fill);
        DrawText(modules[i], (int)button.x + 12, (int)button.y + 12, 13,
                 selectedModule == i ? UI_BACKGROUND : UI_WHITE);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            selectedModule = i;
    }

    DrawLine(20, 532, 230, 532, UI_MUTED);
    DrawText("ALGOQUEST v1.2", 30, 545, 13, UI_CYAN);
    DrawText("Developed in C", 30, 565, 11, UI_MUTED);
}

static void DrawDashboard(void)
{
    DrawSidebar();

    DrawText("Welcome to AlgoQuest", 285, 35, 30, UI_WHITE);
    DrawText("Explore. Visualize. Master Algorithms.", 285, 80, 16, UI_MUTED);

    const char *titles[] = {"SORTING", "STACK", "QUEUE",
                            "LINKED LIST", "DOUBLY LINKED", "TREES"};
    const char *descriptions[] = {"Visualize sorting algorithms",
                                 "Practice PUSH, POP and PEEK", "Explore ENQUEUE and DEQUEUE",
                                 "Understand node connections", "Explore bidirectional links",
                                 "Explore tree operations"};
    Color accents[] = {UI_CYAN, UI_PURPLE, UI_CYAN, UI_PURPLE, UI_CYAN, UI_PURPLE};

    for (int i = 0; i < 6; i++)
    {
        int col = i % 3;
        int row = i / 3;
        Rectangle card = {285 + col * 290, 145 + row * 215, 265, 180};
        int hover = CheckCollisionPointRec(GetMousePosition(), card);
        DrawRectangleRounded(card, 0.12f, 8,
                            hover ? (Color){39, 48, 72, 255} : UI_CARD);
        DrawRectangle((int)card.x, (int)card.y, 265, 5, accents[i]);
        DrawText(titles[i], (int)card.x + 18, (int)card.y + 30, 18, accents[i]);
        DrawText(descriptions[i], (int)card.x + 18, (int)card.y + 75, 13, UI_WHITE);
        DrawText("OPEN MODULE", (int)card.x + 18, (int)card.y + 135, 12, UI_MUTED);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (i == 0)
                selectedModule = 1; // Sorting
            else if (i == 1)
                selectedModule = 2; // Stack
            else if (i == 2)
                selectedModule = 3; // Queue
            else if (i == 3)
                selectedModule = 4; // Linked List
            else if (i == 4)
                selectedModule = 5; // Doubly Linked List
            else
                selectedModule = 6; // Trees / placeholders
        }
    }

    DrawText("Phase 2: Stack, Queue, and Linked List visualizers", 285, 600, 19, UI_WHITE);
    DrawText("Select a module from the sidebar or dashboard cards.",
             285, 635, 15, UI_MUTED);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AlgoQuest - Data Structures Visualizer");
    SetTargetFPS(60);

    StackVisualizerInit();
    QueueVisualizerInit();
    LinkedListVisualizerInit();
    DLLVisualizerInit();

    while (!WindowShouldClose())
    {
        // --- UPDATE LOGIC ---
        if (selectedModule == 2)
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

        // --- DRAW LOGIC ---
        BeginDrawing();
        ClearBackground(UI_BACKGROUND);

        if (selectedModule == 0)
        {
            DrawDashboard();
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
        else
        {
            // For modules not yet implemented
            DrawText(modules[selectedModule], 285, 45, 30, UI_CYAN);
            DrawText("This module will be implemented in a later phase.",
                     285, 110, 18, UI_WHITE);
        }

        DrawFPS(1120, 15);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}