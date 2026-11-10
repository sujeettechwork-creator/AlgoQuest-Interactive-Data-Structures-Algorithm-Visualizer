#include "graph_visualizer.h"
#include "../structures/graph.h"

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

static Graph graph;

static char edgeInput[3] = "";
static char startInput[2] = "";

static int edgeLength = 0;
static int startLength = 0;
static int activeInput = 0;
static int wantsDashboard = 0;

static int traversal[GRAPH_MAX_VERTICES];
static int traversalCount = 0;
static char traversalName[20] = "None";

static char message[100] = "Add vertices to create a graph.";
static Color messageColor;

static Rectangle edgeBox = {285, 115, 90, 35};
static Rectangle startBox = {500, 115, 65, 35};

static Rectangle addVertexButton = {285, 165, 125, 38};
static Rectangle addEdgeButton = {425, 165, 125, 38};
static Rectangle bfsButton = {565, 165, 85, 38};
static Rectangle dfsButton = {665, 165, 85, 38};
static Rectangle clearButton = {765, 165, 100, 38};

static Vector2 positions[GRAPH_MAX_VERTICES];

static int MouseOver(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

static void SetMessage(const char *text, Color color)
{
    snprintf(message, sizeof(message), "%s", text);
    messageColor = color;
}

static int GetVertexIndex(char label)
{
    label = (char)toupper((unsigned char)label);

    if (label < 'A' || label >= 'A' + graph.vertexCount)
        return -1;

    return label - 'A';
}

static void DrawButton(Rectangle rect, const char *label)
{
    Color fill = MouseOver(rect) ? SKYBLUE : LIGHTGRAY;

    DrawRectangleRounded(rect, 0.15f, 6, fill);
    DrawRectangleRoundedLines(rect, 0.15f, 6, DARKGRAY);

    int width = MeasureText(label, 15);

    DrawText(label,
             (int)(rect.x + (rect.width - width) / 2),
             (int)(rect.y + 11),
             15, BLACK);
}

static void CalculatePositions(void)
{
    // Positioned on the right side to avoid overlapping with left-hand controls and matrices
    float centerX = 830.0f;
    float centerY = 430.0f;
    float radius = graph.vertexCount <= 4 ? 120.0f : 145.0f;

    for (int i = 0; i < graph.vertexCount; i++) {
        float angle = -PI / 2.0f +
                      (2.0f * PI * i / graph.vertexCount);

        positions[i].x = centerX + cosf(angle) * radius;
        positions[i].y = centerY + sinf(angle) * radius;
    }
}

static void DrawGraph(void)
{
    CalculatePositions();

    // Draw edges first.
    for (int i = 0; i < graph.vertexCount; i++) {
        for (int j = i + 1; j < graph.vertexCount; j++) {
            if (graph.adj[i][j]) {
                DrawLineEx(positions[i], positions[j], 3, GRAY);
            }
        }
    }

    // Draw vertices.
    for (int i = 0; i < graph.vertexCount; i++) {
        DrawCircleV(positions[i], 25, SKYBLUE);
        DrawCircleLinesV(positions[i], 25, DARKBLUE);

        char label[2] = {(char)('A' + i), '\0'};
        int width = MeasureText(label, 21);

        DrawText(label,
                 (int)positions[i].x - width / 2,
                 (int)positions[i].y - 11,
                 21, BLACK);
    }

    if (graph.vertexCount == 0) {
        const char *placeholder = "Your graph will appear here.";
        int width = MeasureText(placeholder, 18);
        DrawText(placeholder, 830 - width / 2, 430, 18, GRAY);
    }
}

static void DrawAdjacencyMatrix(void)
{
    int x = 285;
    int y = 265;
    int cell = 27;

    DrawText("ADJACENCY MATRIX", x, y - 25, 16, DARKBLUE);

    for (int i = 0; i < graph.vertexCount; i++) {
        char label[2] = {(char)('A' + i), '\0'};

        DrawText(label, x + 35 + i * cell + 8, y, 14, DARKBLUE);
        DrawText(label, x + 10, y + 25 + i * cell, 14, DARKBLUE);
    }

    for (int i = 0; i < graph.vertexCount; i++) {
        for (int j = 0; j < graph.vertexCount; j++) {
            int cx = x + 35 + j * cell;
            int cy = y + 22 + i * cell;

            DrawRectangle(cx, cy, 24, 24,
                          graph.adj[i][j] ? SKYBLUE : LIGHTGRAY);

            DrawRectangleLines(cx, cy, 24, 24, GRAY);

            char value[2] = {
                graph.adj[i][j] ? '1' : '0', '\0'
            };

            DrawText(value, cx + 7, cy + 4, 14, BLACK);
        }
    }
}

static void DrawTraversal(void)
{
    int x = 285;
    int y = 545;

    DrawText("TRAVERSAL RESULT", x, y, 17, DARKBLUE);
    DrawText(traversalName, x, y + 24, 16, MAROON);

    if (traversalCount == 0) {
        DrawText("Run BFS or DFS to see the order.", x, y + 52, 14, GRAY);
        return;
    }

    char result[120] = "";

    for (int i = 0; i < traversalCount; i++) {
        char part[12];

        snprintf(part, sizeof(part), "%c%s",
                 'A' + traversal[i],
                 i == traversalCount - 1 ? "" : " -> ");

        strncat(result, part, sizeof(result) - strlen(result) - 1);
    }

    DrawText(result, x, y + 52, 15, DARKGREEN);
}

static void RunTraversal(int useBFS)
{
    if (graph.vertexCount == 0) {
        SetMessage("Add vertices before running traversal.", RED);
        return;
    }

    if (startLength != 1) {
        SetMessage("Enter a starting vertex, such as A.", RED);
        return;
    }

    int start = GetVertexIndex(startInput[0]);

    if (start < 0) {
        SetMessage("Starting vertex does not exist.", RED);
        return;
    }

    if (useBFS) {
        traversalCount = GraphBFS(&graph, start, traversal);
        snprintf(traversalName, sizeof(traversalName), "BFS from %c",
                 'A' + start);
    } else {
        traversalCount = GraphDFS(&graph, start, traversal);
        snprintf(traversalName, sizeof(traversalName), "DFS from %c",
                 'A' + start);
    }

    SetMessage("Traversal completed successfully.", GREEN);
}

void GraphVisualizerInit(void)
{
    GraphInit(&graph);

    edgeInput[0] = '\0';
    startInput[0] = '\0';

    edgeLength = 0;
    startLength = 0;
    activeInput = 0;
    wantsDashboard = 0;

    traversalCount = 0;
    snprintf(traversalName, sizeof(traversalName), "None");

    SetMessage("Add vertices to create a graph.", DARKGRAY);
}

void GraphVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        wantsDashboard = 1;
        return;
    }

    if (activeInput != 0) {
        int key = GetCharPressed();

        while (key > 0) {
            if (activeInput == 1 && edgeLength < 2 &&
                isalpha((unsigned char)key)) {
                edgeInput[edgeLength++] =
                    (char)toupper((unsigned char)key);
                edgeInput[edgeLength] = '\0';
            }

            if (activeInput == 2 && startLength < 1 &&
                isalpha((unsigned char)key)) {
                startInput[startLength++] =
                    (char)toupper((unsigned char)key);
                startInput[startLength] = '\0';
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (activeInput == 1 && edgeLength > 0) {
                edgeInput[--edgeLength] = '\0';
            }

            if (activeInput == 2 && startLength > 0) {
                startInput[--startLength] = '\0';
            }
        }
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    if (MouseOver(edgeBox)) {
        activeInput = 1;
    }
    else if (MouseOver(startBox)) {
        activeInput = 2;
    }
    else if (MouseOver(addVertexButton)) {
        if (GraphAddVertex(&graph)) {
            char text[80];

            snprintf(text, sizeof(text), "Vertex %c added.",
                     'A' + graph.vertexCount - 1);

            SetMessage(text, GREEN);
            traversalCount = 0;
        } else {
            SetMessage("Maximum of 8 vertices reached.", RED);
        }
    }
    else if (MouseOver(addEdgeButton)) {
        if (edgeLength != 2) {
            SetMessage("Enter two vertices, such as AB.", RED);
        } else {
            int from = GetVertexIndex(edgeInput[0]);
            int to = GetVertexIndex(edgeInput[1]);

            if (GraphAddEdge(&graph, from, to)) {
                SetMessage("Edge added successfully.", GREEN);
                traversalCount = 0;
            } else {
                SetMessage("Invalid vertices or self-edge.", RED);
            }
        }

        edgeInput[0] = '\0';
        edgeLength = 0;
        activeInput = 0;
    }
    else if (MouseOver(bfsButton)) {
        RunTraversal(1);
        activeInput = 0;
    }
    else if (MouseOver(dfsButton)) {
        RunTraversal(0);
        activeInput = 0;
    }
    else if (MouseOver(clearButton)) {
        GraphClear(&graph);

        edgeInput[0] = '\0';
        startInput[0] = '\0';

        edgeLength = 0;
        startLength = 0;
        activeInput = 0;
        traversalCount = 0;

        SetMessage("Graph cleared.", GREEN);
    }
    else {
        activeInput = 0;
    }
}

void GraphVisualizerDraw(void)
{
    DrawText("GRAPH ALGORITHMS VISUALIZER",
             285, 35, 26, DARKBLUE);

    DrawText("Edge (e.g. AB):", 285, 90, 16, DARKGRAY);
    DrawRectangleRec(edgeBox, RAYWHITE);
    DrawRectangleLinesEx(edgeBox, 1,
                         activeInput == 1 ? BLUE : GRAY);
    DrawText(edgeInput, 295, 124, 17, BLACK);

    DrawText("Start:", 500, 90, 16, DARKGRAY);
    DrawRectangleRec(startBox, RAYWHITE);
    DrawRectangleLinesEx(startBox, 1,
                         activeInput == 2 ? BLUE : GRAY);
    DrawText(startInput, 510, 124, 17, BLACK);

    DrawButton(addVertexButton, "ADD VERTEX");
    DrawButton(addEdgeButton, "ADD EDGE");
    DrawButton(bfsButton, "RUN BFS");
    DrawButton(dfsButton, "RUN DFS");
    DrawButton(clearButton, "CLEAR");

    DrawText(message, 285, 220, 17, messageColor);

    DrawGraph();
    DrawAdjacencyMatrix();
    DrawTraversal();

    DrawText("Undirected Graph | Maximum 8 vertices",
             285, 715, 13, GRAY);
}

int GraphVisualizerWantsDashboard(void)
{
    return wantsDashboard;
}