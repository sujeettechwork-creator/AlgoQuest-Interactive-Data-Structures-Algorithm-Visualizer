#include "circular_linked_list_visualizer.h"
#include "list_view.h"

static ListView view;

void CLLVisualizerInit(void) { ListViewInit(&view, LV_CIRCULAR); }
void CLLVisualizerDraw(void) { ListViewDraw(&view); }
