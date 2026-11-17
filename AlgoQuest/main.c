#include "raylib.h"
#include "graphics/ui.h"
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
#include "graphics/challenge_visualizer.h"

#include <math.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750
#define SIDEBAR_WIDTH 250

typedef struct
{
    const char *name;
    const char *desc;
    const char *tag;
    int accent; /* 0 cyan, 1 purple, 2 amber */
    void (*init)(void);
    void (*draw)(void);
} Module;

static const Module modules[] = {
    {"Sorting Visualizer", "Bubble, Selection, Insertion,\nMerge and Quick sort animated", "O(n log n)", 0, SortingVisualizerInit, SortingVisualizerDraw},
    {"Stack Visualizer", "LIFO with push, pop and peek\nplus overflow detection", "O(1) ops", 1, StackVisualizerInit, StackVisualizerDraw},
    {"Queue Visualizer", "Linear vs circular queues with\nFRONT and REAR pointers", "O(1) ops", 0, QueueVisualizerInit, QueueVisualizerDraw},
    {"Linked List", "Nodes, pointers and a live\nmemory layout view", "O(n) search", 1, LinkedListVisualizerInit, LinkedListVisualizerDraw},
    {"Doubly Linked List", "Prev / next links with\nbidirectional arrows", "O(n) search", 0, DLLVisualizerInit, DLLVisualizerDraw},
    {"Circular Linked List", "Tail links back to head,\nreverse and insert anywhere", "O(n) search", 1, CLLVisualizerInit, CLLVisualizerDraw},
    {"Binary Search Tree", "Animated search paths and\nthree traversal orders", "O(h) ops", 0, BSTVisualizerInit, BSTVisualizerDraw},
    {"AVL Tree", "Self-balancing tree with\nlive balance factors", "O(log n)", 1, AVLVisualizerInit, AVLVisualizerDraw},
    {"Heap & Priority Queue", "Min / max heap, tree and\narray views side by side", "O(log n)", 0, HeapVisualizerInit, HeapVisualizerDraw},
    {"Graph Algorithms", "Drag vertices, run BFS and\nDFS step by step", "O(V + E)", 1, GraphVisualizerInit, GraphVisualizerDraw},
    {"AlgoQuest Challenge", "Timed quiz: test what you\nhave learned so far", "10 questions", 2, ChallengeVisualizerInit, ChallengeVisualizerDraw}};

#define MODULE_COUNT ((int)(sizeof(modules) / sizeof(modules[0])))

static int selected = 0; /* 0 = dashboard, 1..MODULE_COUNT = modules */
static float fade = 1.0f;
static float dashT = 0.0f;
static float sideHover[MODULE_COUNT + 1];

static Color Accent(int id)
{
    switch (id)
    {
    case 0: return C_CYAN;
    case 1: return C_PURPLE;
    default: return C_AMBER;
    }
}

static void Select(int index)
{
    if (index == selected)
        return;

    selected = index;
    fade = 1.0f;

    if (index == 0)
        dashT = 0.0f;
}

static void DrawSidebar(void)
{
    DrawRectangleGradientV(0, 0, SIDEBAR_WIDTH, SCREEN_HEIGHT, (Color){16, 21, 40, 255}, (Color){10, 13, 26, 255});
    DrawLine(SIDEBAR_WIDTH - 1, 0, SIDEBAR_WIDTH - 1, SCREEN_HEIGHT, Fade(C_BORDER, 0.8f));

    float pulse = 0.5f + 0.5f * sinf(UITime() * 2.0f);
    DrawCircleV((Vector2){40, 36}, 22.0f + 3.0f * pulse, Fade(C_CYAN, 0.10f));
    DrawText("ALGO", 30, 20, 28, C_CYAN);
    DrawText("QUEST", 106, 20, 28, C_WHITE);
    DrawText("LEARN DATA STRUCTURES", 30, 54, 10, C_MUTED);

    Vector2 mouse = GetMousePosition();

    for (int i = 0; i <= MODULE_COUNT; i++)
    {
        const char *name = i == 0 ? "Dashboard" : modules[i - 1].name;
        Color accent = i == 0 ? C_WHITE : Accent(modules[i - 1].accent);
        Rectangle b = {16, 82.0f + i * 40.0f, 218, 34};
        int hover = CheckCollisionPointRec(mouse, b);
        int on = selected == i;

        sideHover[i] = UIApproach(sideHover[i], (hover || on) ? 1.0f : 0.0f, 14.0f);

        float slide = sideHover[i] * 6.0f;
        Rectangle r = {b.x + slide, b.y, b.width - slide, b.height};

        DrawRectangleRounded(r, 0.25f, 8, on ? Fade(accent, 0.95f) : Fade(C_PANEL2, 0.75f * sideHover[i]));

        if (!on)
            DrawRectangle((int)b.x, (int)b.y + 7, 3, 20, Fade(accent, 0.8f * sideHover[i]));

        DrawText(name, (int)r.x + 14, (int)r.y + 10, 13, on ? UITextOn(accent) : (hover ? C_WHITE : C_MUTED));

        if (hover)
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            Select(i);
    }

    DrawLine(20, 580, 230, 580, Fade(C_BORDER, 0.8f));
    DrawText("TIP", 30, 596, 11, C_CYAN);
    DrawText("ESC returns to the\ndashboard from any module.", 30, 614, 11, C_MUTED);
    DrawText("ALGOQUEST v2.0", 30, 690, 13, C_CYAN);
    DrawText("Developed in C + raylib", 30, 708, 11, C_MUTED);
    DrawFPS(176, 726);
}

static void DrawDashboard(void)
{
    UIBackground();

    dashT += GetFrameTime();

    DrawText("Welcome to AlgoQuest", 40, 22, 36, C_WHITE);
    DrawText("Explore. Visualize. Master Algorithms.", 40, 66, 16, C_MUTED);
    DrawRectangleGradientH(40, 90, 380, 2, C_CYAN, Fade(C_CYAN, 0.0f));

    for (int i = 0; i < MODULE_COUNT; i++)
    {
        const Module *m = &modules[i];
        Color accent = Accent(m->accent);

        float e = UIClampf((dashT - i * 0.05f) * 4.0f, 0.0f, 1.0f);
        e = 1.0f - (1.0f - e) * (1.0f - e);

        int col = i % 3;
        int row = i / 3;
        Rectangle card = {30.0f + col * 300.0f, 112.0f + row * 148.0f + (1.0f - e) * 24.0f, 290, 134};
        int hover = e > 0.99f && CheckCollisionPointRec(GetMousePosition(), card);

        if (hover)
        {
            card.y -= 4;
            SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            DrawRectangleRounded((Rectangle){card.x - 5, card.y - 5, card.width + 10, card.height + 10}, 0.12f, 8, Fade(accent, 0.18f));
        }

        DrawRectangleRounded((Rectangle){card.x + 3, card.y + 6, card.width, card.height}, 0.1f, 8, Fade(BLACK, 0.4f * e));
        DrawRectangleRounded(card, 0.1f, 8, Fade(hover ? C_PANEL2 : C_PANEL, e));
        DrawRectangleRoundedLinesEx(card, 0.1f, 8, 1.0f, Fade(hover ? accent : C_BORDER, e));
        DrawRectangleRounded((Rectangle){card.x + 14, card.y, 60, 4}, 0.5f, 4, Fade(accent, e));

        DrawText(TextFormat("%02d", i + 1), (int)card.x + 236, (int)card.y + 14, 22, Fade(accent, 0.35f * e));
        DrawText(m->name, (int)card.x + 16, (int)card.y + 18, 18, Fade(accent, e));
        DrawText(m->desc, (int)card.x + 16, (int)card.y + 50, 13, Fade(C_WHITE, e));

        Rectangle chip = {card.x + 16, card.y + 100, (float)MeasureText(m->tag, 12) + 18, 22};
        DrawRectangleRounded(chip, 0.5f, 6, Fade(accent, 0.18f * e));
        DrawText(m->tag, (int)chip.x + 9, (int)chip.y + 5, 12, Fade(accent, e));
        DrawText("OPEN >", (int)card.x + 222, (int)card.y + 104, 12, Fade(hover ? accent : C_MUTED, e));

        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            Select(i + 1);
    }

    DrawText("Select a module from the sidebar or click a card.   Press ESC anywhere to come back here.", 40, 722, 13, C_MUTED);
}

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AlgoQuest - Data Structures & Algorithms Visualizer");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    for (int i = 0; i < MODULE_COUNT; i++)
        modules[i].init();

    while (!WindowShouldClose())
    {
        UIFrame();

        if (IsKeyPressed(KEY_ESCAPE) && selected != 0)
            Select(0);

        fade = UIApproach(fade, 0.0f, 9.0f);

        BeginDrawing();
        ClearBackground(C_BG);

        /* content is drawn in a local 950x750 space to the right of the sidebar */
        SetMouseOffset(-SIDEBAR_WIDTH, 0);
        BeginScissorMode(SIDEBAR_WIDTH, 0, CW, SCREEN_HEIGHT);

        Camera2D cam = {0};
        cam.offset = (Vector2){(float)SIDEBAR_WIDTH, 0.0f};
        cam.zoom = 1.0f;
        BeginMode2D(cam);

        if (selected == 0)
            DrawDashboard();
        else
            modules[selected - 1].draw();

        if (fade > 0.01f)
            DrawRectangle(0, 0, CW, CH, Fade(C_BG, fade));

        EndMode2D();
        EndScissorMode();
        SetMouseOffset(0, 0);

        DrawSidebar();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
