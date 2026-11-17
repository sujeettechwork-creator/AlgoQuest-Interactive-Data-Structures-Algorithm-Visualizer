#include "doubly_linked_list_visualizer.h"
#include "list_view.h"

static ListView view;

void DLLVisualizerInit(void) { ListViewInit(&view, LV_DOUBLY); }
void DLLVisualizerDraw(void) { ListViewDraw(&view); }
