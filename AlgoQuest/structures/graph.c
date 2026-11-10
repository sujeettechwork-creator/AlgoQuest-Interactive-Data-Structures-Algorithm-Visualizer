
#include "graph.h"

static void DFSVisit(const Graph *graph, int vertex,
                     int visited[], int result[], int *count)
{
    visited[vertex] = 1;
    result[(*count)++] = vertex;

    for (int i = 0; i < graph->vertexCount; i++) {
        if (graph->adj[vertex][i] && !visited[i]) {
            DFSVisit(graph, i, visited, result, count);
        }
    }
}

void GraphInit(Graph *graph)
{
    GraphClear(graph);
}

int GraphAddVertex(Graph *graph)
{
    if (graph->vertexCount >= GRAPH_MAX_VERTICES)
        return 0;

    graph->vertexCount++;
    return 1;
}

int GraphAddEdge(Graph *graph, int from, int to)
{
    if (from < 0 || to < 0 ||
        from >= graph->vertexCount ||
        to >= graph->vertexCount ||
        from == to)
        return 0;

    graph->adj[from][to] = 1;
    graph->adj[to][from] = 1;

    return 1;
}

void GraphClear(Graph *graph)
{
    graph->vertexCount = 0;

    for (int i = 0; i < GRAPH_MAX_VERTICES; i++) {
        for (int j = 0; j < GRAPH_MAX_VERTICES; j++) {
            graph->adj[i][j] = 0;
        }
    }
}

int GraphBFS(const Graph *graph, int start, int result[])
{
    if (start < 0 || start >= graph->vertexCount)
        return 0;

    int visited[GRAPH_MAX_VERTICES] = {0};
    int queue[GRAPH_MAX_VERTICES];
    int front = 0;
    int rear = 0;
    int count = 0;

    visited[start] = 1;
    queue[rear++] = start;

    while (front < rear) {
        int current = queue[front++];
        result[count++] = current;

        for (int i = 0; i < graph->vertexCount; i++) {
            if (graph->adj[current][i] && !visited[i]) {
                visited[i] = 1;
                queue[rear++] = i;
            }
        }
    }

    return count;
}

int GraphDFS(const Graph *graph, int start, int result[])
{
    if (start < 0 || start >= graph->vertexCount)
        return 0;

    int visited[GRAPH_MAX_VERTICES] = {0};
    int count = 0;

    DFSVisit(graph, start, visited, result, &count);

    return count;
}
