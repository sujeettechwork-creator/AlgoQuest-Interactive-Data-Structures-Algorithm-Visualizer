#include "linked_list_visualizer.h"
#include "list_view.h"

static ListView view;

void LinkedListVisualizerInit(void) { ListViewInit(&view, LV_SINGLY); }
void LinkedListVisualizerDraw(void) { ListViewDraw(&view); }
