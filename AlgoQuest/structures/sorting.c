
#include "sorting.h"
#include <string.h>

static SortFrame frames[SORT_MAX_FRAMES];
static int frameCount = 0;
static int itemCount = 0;
static int comparisons = 0;
static int writes = 0;

static void SaveFrame(const int a[], int activeA, int activeB)
{
    if (frameCount >= SORT_MAX_FRAMES)
        return;

    SortFrame *frame = &frames[frameCount++];

    memcpy(frame->values, a, sizeof(int) * itemCount);

    frame->activeA = activeA;
    frame->activeB = activeB;
    frame->comparisons = comparisons;
    frame->writes = writes;
}

static void Swap(int a[], int i, int j)
{
    if (i == j)
        return;

    int temp = a[i];
    a[i] = a[j];
    a[j] = temp;

    writes += 2;
    SaveFrame(a, i, j);
}

static void BubbleSort(int a[], int n)
{
    for (int i = 0; i < n - 1; i++) {
        int changed = 0;

        for (int j = 0; j < n - i - 1; j++) {
            comparisons++;

            if (a[j] > a[j + 1]) {
                Swap(a, j, j + 1);
                changed = 1;
            }
        }

        if (!changed)
            break;
    }
}

static void SelectionSort(int a[], int n)
{
    for (int i = 0; i < n - 1; i++) {
        int minimum = i;

        for (int j = i + 1; j < n; j++) {
            comparisons++;

            if (a[j] < a[minimum])
                minimum = j;
        }

        Swap(a, i, minimum);
    }
}

static void InsertionSort(int a[], int n)
{
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;

        while (j >= 0) {
            comparisons++;

            if (a[j] <= key)
                break;

            a[j + 1] = a[j];
            writes++;

            SaveFrame(a, j, j + 1);
            j--;
        }

        a[j + 1] = key;
        writes++;

        SaveFrame(a, j + 1, i);
    }
}

static void Merge(int a[], int left, int mid, int right)
{
    int temp[SORT_MAX_ITEMS];
    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {
        comparisons++;

        if (a[i] <= a[j])
            temp[k++] = a[i++];
        else
            temp[k++] = a[j++];
    }

    while (i <= mid)
        temp[k++] = a[i++];

    while (j <= right)
        temp[k++] = a[j++];

    for (i = 0; i < k; i++) {
        a[left + i] = temp[i];
        writes++;

        SaveFrame(a, left + i, right);
    }
}

static void MergeSort(int a[], int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    MergeSort(a, left, mid);
    MergeSort(a, mid + 1, right);
    Merge(a, left, mid, right);
}

static int Partition(int a[], int low, int high)
{
    int pivot = a[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        comparisons++;

        if (a[j] < pivot) {
            i++;
            Swap(a, i, j);
        }
    }

    Swap(a, i + 1, high);

    return i + 1;
}

static void QuickSort(int a[], int low, int high)
{
    if (low >= high)
        return;

    int pivot = Partition(a, low, high);

    QuickSort(a, low, pivot - 1);
    QuickSort(a, pivot + 1, high);
}

void SortGenerate(SortAlgorithm algorithm,
                  const int values[], int count)
{
    int a[SORT_MAX_ITEMS];

    if (count < 1)
        return;

    if (count > SORT_MAX_ITEMS)
        count = SORT_MAX_ITEMS;

    memcpy(a, values, sizeof(int) * count);

    itemCount = count;
    frameCount = 0;
    comparisons = 0;
    writes = 0;

    SaveFrame(a, -1, -1);

    switch (algorithm) {
        case SORT_BUBBLE:
            BubbleSort(a, count);
            break;

        case SORT_SELECTION:
            SelectionSort(a, count);
            break;

        case SORT_INSERTION:
            InsertionSort(a, count);
            break;

        case SORT_MERGE:
            MergeSort(a, 0, count - 1);
            break;

        case SORT_QUICK:
            QuickSort(a, 0, count - 1);
            break;
    }

    SaveFrame(a, -1, -1);
}

const SortFrame *SortGetFrames(void)
{
    return frames;
}

int SortGetFrameCount(void)
{
    return frameCount;
}
