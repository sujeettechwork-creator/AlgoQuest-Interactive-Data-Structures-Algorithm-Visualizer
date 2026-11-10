
#ifndef GRAPH_H
#define GRAPH_H

#define GRAPH_MAX_VERTICES 8

typedef struct {
    int vertexCount;
    int adj[GRAPH_MAX_VERTICES][GRAPH_MAX_VERTICES];
} Graph;

void GraphInit(Graph *graph);
int GraphAddVertex(Graph *graph);
int GraphAddEdge(Graph *graph, int from, int to);
void GraphClear(Graph *graph);
int GraphBFS(const Graph *graph, int start, int result[]);
int GraphDFS(const Graph *graph, int start, int result[]);

#endif
