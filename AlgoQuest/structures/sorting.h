
#ifndef SORTING_H
#define SORTING_H

#define SORT_MAX_ITEMS 30
#define SORT_MAX_FRAMES 4000

typedef enum {
    SORT_BUBBLE,
    SORT_SELECTION,
    SORT_INSERTION,
    SORT_MERGE,
    SORT_QUICK
} SortAlgorithm;

typedef struct {
    int values[SORT_MAX_ITEMS];
    int activeA;
    int activeB;
    int comparisons;
    int writes;
} SortFrame;

void SortGenerate(SortAlgorithm algorithm, const int values[], int count);
const SortFrame *SortGetFrames(void);
int SortGetFrameCount(void);

#endif
