#include "graph_visualizer.h"
#include "ui.h"
#include "../structures/graph.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define NODE_R 24.0f

static Graph graph;
static Vector2 pos[GRAPH_MAX_VERTICES];
static char edgeBuf[4];
static char startBuf[3];
static int activeEdge = 0;
static int activeStart = 1;
static UIStatus st;

static int trav[GRAPH_MAX_VERTICES];
static int travCount = 0;
static float travProgress = 0.0f;
static int travBFS = 1;
static int dragging = -1;
static int edgeFrom = -1;

static const Rectangle canvas = {325, 100, 595, 420};

static Vector2 SlotPos(int i)
{
    float angle = -PI / 2.0f + 2.0f * PI * (float)i / (float)GRAPH_MAX_VERTICES;
    return (Vector2){canvas.x + canvas.width / 2 + cosf(angle) * 160.0f,
                     canvas.y + canvas.height / 2 + sinf(angle) * 150.0f};
}

void GraphVisualizerInit(void)
{
    GraphInit(&graph);
    edgeBuf[0] = startBuf[0] = '\0';
    activeEdge = 0;
    activeStart = 1;
    travCount = 0;
    travProgress = 0.0f;
    dragging = edgeFrom = -1;

    for (int i = 0; i < GRAPH_MAX_VERTICES; i++)
        pos[i] = SlotPos(i);

    UISetStatus(&st, "Add vertices, connect them, then run BFS or DFS.", C_CYAN);
}

static int VertexIndex(char label)
{
    int i = toupper((unsigned char)label) - 'A';
    return (i >= 0 && i < graph.vertexCount) ? i : -1;
}

static int VertexAt(Vector2 p)
{
    for (int i = graph.vertexCount - 1; i >= 0; i--)
    {
        float dx = p.x - pos[i].x, dy = p.y - pos[i].y;
        if (dx * dx + dy * dy <= NODE_R * NODE_R)
            return i;
    }
    return -1;
}

static void AddEdge(int a, int b)
{
    if (a < 0 || b < 0)
        UISetStatus(&st, "Both vertices must exist (e.g. AB).", C_RED);
    else if (a == b)
        UISetStatus(&st, "Self-loops are not allowed.", C_RED);
    else if (graph.adj[a][b])
        UISetStatus(&st, "That edge already exists.", C_AMBER);
    else if (GraphAddEdge(&graph, a, b))
    {
        travCount = 0;
        UISetStatus(&st, TextFormat("Edge %c - %c added.", 'A' + a, 'A' + b), C_GREEN);
    }
}

static void RunTraversal(int bfs)
{
    if (graph.vertexCount == 0)
    {
        UISetStatus(&st, "Add some vertices first.", C_RED);
        return;
    }

    int start = 0;

    if (startBuf[0] != '\0')
    {
        start = VertexIndex(startBuf[0]);

        if (start < 0)
        {
            UISetStatus(&st, "That start vertex does not exist.", C_RED);
            return;
        }
    }

    travCount = bfs ? GraphBFS(&graph, start, trav) : GraphDFS(&graph, start, trav);
    travProgress = 0.0f;
    travBFS = bfs;
    UISetStatus(&st, TextFormat("%s from %c: visited %d vertex(es).", bfs ? "BFS" : "DFS", 'A' + start, travCount), C_PURPLE);
}

static void RandomGraph(void)
{
    GraphClear(&graph);
    travCount = 0;

    for (int i = 0; i < 6; i++)
    {
        GraphAddVertex(&graph);
        pos[i] = SlotPos(i);
    }

    for (int v = 1; v < 6; v++)
        GraphAddEdge(&graph, v, GetRandomValue(0, v - 1));

    for (int i = 0; i < 3; i++)
        GraphAddEdge(&graph, GetRandomValue(0, 5), GetRandomValue(0, 5));

    UISetStatus(&st, "Random connected graph created. Drag vertices to rearrange.", C_GREEN);
}

void GraphVisualizerDraw(void)
{
    Vector2 mouse = GetMousePosition();

    UIBackground();
    UIHeader("GRAPH ALGORITHMS", "Undirected graph   |   drag vertices, right-click two vertices to connect them", C_GREEN);

    /* ---- controls ---- */
    UICard((Rectangle){30, 100, 280, 300}, "CONTROLS", C_GREEN);

    DrawText("Edge (e.g. AB)", 50, 135, 13, C_MUTED);
    if (UIInput((Rectangle){50, 155, 110, 42}, edgeBuf, sizeof(edgeBuf), &activeEdge, "AB", 1))
        AddEdge(VertexIndex(edgeBuf[0]), edgeBuf[1] ? VertexIndex(edgeBuf[1]) : -1);
    if (UIButton((Rectangle){170, 155, 120, 42}, "ADD EDGE", C_CYAN))
    {
        if (strlen(edgeBuf) == 2)
            AddEdge(VertexIndex(edgeBuf[0]), VertexIndex(edgeBuf[1]));
        else
            UISetStatus(&st, "Type two vertex letters, such as AB.", C_RED);
        edgeBuf[0] = '\0';
    }

    DrawText("Start vertex", 50, 212, 13, C_MUTED);
    UIInput((Rectangle){50, 232, 110, 42}, startBuf, sizeof(startBuf), &activeStart, "A", 1);
    if (UIButton((Rectangle){170, 232, 58, 42}, "BFS", C_PURPLE))
        RunTraversal(1);
    if (UIButton((Rectangle){232, 232, 58, 42}, "DFS", C_BLUE))
        RunTraversal(0);

    if (UIButton((Rectangle){50, 292, 240, 42}, "ADD VERTEX", C_GREEN))
    {
        if (graph.vertexCount < GRAPH_MAX_VERTICES && GraphAddVertex(&graph))
        {
            pos[graph.vertexCount - 1] = SlotPos(graph.vertexCount - 1);
            travCount = 0;
            UISetStatus(&st, TextFormat("Vertex %c added.", 'A' + graph.vertexCount - 1), C_GREEN);
        }
        else
            UISetStatus(&st, "Maximum of 8 vertices reached.", C_RED);
    }

    if (UIButton((Rectangle){50, 345, 115, 42}, "RANDOM", C_AMBER))
        RandomGraph();
    if (UIButton((Rectangle){175, 345, 115, 42}, "CLEAR", C_RED))
    {
        GraphClear(&graph);
        travCount = 0;
        edgeFrom = dragging = -1;
        UISetStatus(&st, "Graph cleared.", C_AMBER);
    }

    /* ---- adjacency matrix ---- */
    UICard((Rectangle){30, 415, 280, 240}, "ADJACENCY MATRIX", C_CYAN);
    {
        int cell = 23;
        int mx = 66, my = 466;

        for (int i = 0; i < graph.vertexCount; i++)
        {
            DrawText(TextFormat("%c", 'A' + i), mx + i * cell + 7, my - 18, 13, C_CYAN);
            DrawText(TextFormat("%c", 'A' + i), mx - 20, my + i * cell + 5, 13, C_CYAN);
        }

        for (int i = 0; i < graph.vertexCount; i++)
            for (int j = 0; j < graph.vertexCount; j++)
            {
                Rectangle r = {(float)(mx + j * cell), (float)(my + i * cell), (float)cell - 2, (float)cell - 2};
                int on = graph.adj[i][j];

                DrawRectangleRounded(r, 0.2f, 4, on ? Fade(C_CYAN, 0.85f) : Fade(C_BG, 0.9f));
                DrawText(on ? "1" : "0", (int)r.x + 8, (int)r.y + 5, 12, on ? C_BG : C_MUTED);
            }

    }

    /* ---- canvas ---- */
    UICard(canvas, "GRAPH VIEW", C_GREEN);

    /* dragging */
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, canvas))
        dragging = VertexAt(mouse);

    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        dragging = -1;

    if (dragging >= 0)
    {
        pos[dragging].x = UIClampf(mouse.x, canvas.x + NODE_R + 4, canvas.x + canvas.width - NODE_R - 4);
        pos[dragging].y = UIClampf(mouse.y, canvas.y + NODE_R + 30, canvas.y + canvas.height - NODE_R - 4);
        SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
    }
    else if (VertexAt(mouse) >= 0 && CheckCollisionPointRec(mouse, canvas))
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

    /* right-click edge creation */
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        int hit = CheckCollisionPointRec(mouse, canvas) ? VertexAt(mouse) : -1;

        if (hit < 0)
            edgeFrom = -1;
        else if (edgeFrom < 0)
            edgeFrom = hit;
        else
        {
            AddEdge(edgeFrom, hit);
            edgeFrom = -1;
        }
    }

    if (graph.vertexCount == 0)
        DrawText("Your graph will appear here.  Press ADD VERTEX.", 420, 300, 20, Fade(C_MUTED, 0.8f));

    if (travCount > 0)
        travProgress += GetFrameTime() * 2.0f;

    int shown = travCount > 0 ? (int)travProgress : 0;
    if (shown > travCount)
        shown = travCount;

    int order[GRAPH_MAX_VERTICES];
    for (int i = 0; i < GRAPH_MAX_VERTICES; i++)
        order[i] = -1;
    for (int k = 0; k < travCount; k++)
        order[trav[k]] = k;

    /* edges */
    for (int i = 0; i < graph.vertexCount; i++)
        for (int j = i + 1; j < graph.vertexCount; j++)
        {
            if (!graph.adj[i][j])
                continue;

            int lit = 0;

            if (order[i] >= 0 && order[j] >= 0 && order[i] < shown && order[j] < shown)
            {
                int later = order[i] > order[j] ? i : j;
                int earlier = later == i ? j : i;
                int parent = -1;

                /* tree edge heuristic: BFS = earliest visited neighbour, DFS = latest visited neighbour */
                if (travBFS)
                {
                    for (int k = 0; k < order[later] && parent < 0; k++)
                        if (graph.adj[later][trav[k]])
                            parent = trav[k];
                }
                else
                {
                    for (int k = order[later] - 1; k >= 0 && parent < 0; k--)
                        if (graph.adj[later][trav[k]])
                            parent = trav[k];
                }

                lit = parent == earlier;
            }

            DrawLineEx(pos[i], pos[j], lit ? 5.0f : 3.0f, lit ? C_AMBER : (Color){80, 96, 140, 255});
        }

    if (edgeFrom >= 0)
    {
        DrawLineEx(pos[edgeFrom], mouse, 2.0f, Fade(C_GREEN, 0.7f));
        DrawText("click a second vertex with the RIGHT button", (int)canvas.x + 16, (int)canvas.y + 392, 13, C_GREEN);
    }

    /* vertices */
    for (int i = 0; i < graph.vertexCount; i++)
    {
        Color fill = C_PANEL2, border = C_CYAN;
        float glow = 0.0f;

        if (order[i] >= 0 && order[i] < shown)
        {
            fill = (Color){70, 50, 140, 255};
            border = C_PURPLE;

            if (order[i] == shown - 1)
            {
                fill = (Color){120, 84, 20, 255};
                border = C_AMBER;
                glow = 1.0f;
            }
        }

        if (i == edgeFrom)
        {
            border = C_GREEN;
            glow = 1.0f;
        }

        UINode(pos[i], NODE_R, TextFormat("%c", 'A' + i), fill, border, glow);
    }

    DrawText("Left-drag: move   |   Right-click A then B: connect", (int)canvas.x + 16, (int)canvas.y + 32, 13, C_MUTED);

    /* ---- traversal result ---- */
    UICard((Rectangle){325, 535, 595, 120}, "TRAVERSAL RESULT", C_PURPLE);

    if (travCount == 0)
        DrawText("Run BFS or DFS to see the visiting order.", 351, 580, 16, C_MUTED);
    else
    {
        DrawText(travBFS ? "Breadth-First Search  (queue, level by level)" : "Depth-First Search  (stack / recursion, go deep first)",
                 351, 552, 14, C_PURPLE);

        int x = 351;

        for (int k = 0; k < shown; k++)
        {
            Color c = (k == shown - 1) ? C_AMBER : C_CYAN;
            UINode((Vector2){(float)x + 18, 612}, 18, TextFormat("%c", 'A' + trav[k]), C_PANEL2, c, k == shown - 1 ? 1.0f : 0.0f);

            if (k < travCount - 1)
                UIArrow((Vector2){(float)x + 38, 612}, (Vector2){(float)x + 62, 612}, Fade(C_MUTED, 0.8f), 2.0f);

            x += 66;
        }
    }

    UIStatusBar(&st, (Rectangle){30, 670, 890, 68});
}
