#include "sorting_visualizer.h"
#include "../structures/sorting.h"

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ARRAY_SIZE 25

static int original[ARRAY_SIZE];
static int arrayCount = ARRAY_SIZE;

static SortAlgorithm selectedAlgorithm = SORT_BUBBLE;

static const char *algorithmNames[] = {
    "Bubble Sort",
    "Selection Sort",
    "Insertion Sort",
    "Merge Sort",
    "Quick Sort"
};

static const SortFrame *frames = NULL;
static int frameCount = 0;
static int currentFrame = 0;

static float timer = 0.0f;
static float frameDelay = 0.12f;

static int playing = 0;
static int wantsDashboard = 0;
static int generated = 0;

static char status[100] = "Generate an array to begin.";

// Adjusted button widths and positions so text doesn't overlap
static Rectangle generateButton = {285, 105, 125, 38};
static Rectangle startButton = {420, 105, 135, 38};
static Rectangle resetButton = {565, 105, 90, 38};

static Rectangle algorithmButtons[5] = {
    {285, 55, 125, 34},
    {420, 55, 125, 34},
    {555, 55, 125, 34},
    {690, 55, 125, 34},
    {825, 55, 125, 34}
};

static Rectangle speedSlider = {735, 113, 160, 12};

static Color BAR_COLOR = {0, 190, 210, 255};
static Color ACTIVE_COLOR = {255, 170, 50, 255};
static Color SORTED_COLOR = {70, 200, 120, 255};

static int MouseOver(Rectangle rect)
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

static void GenerateRandomArray(void)
{
    for (int i = 0; i < arrayCount; i++)
        original[i] = GetRandomValue(5, 100);

    generated = 0;
    playing = 0;
    currentFrame = 0;
    timer = 0.0f;

    snprintf(status, sizeof(status), "Random array generated.");
}

static void BuildSortingFrames(void)
{
    SortGenerate(selectedAlgorithm, original, arrayCount);

    frames = SortGetFrames();
    frameCount = SortGetFrameCount();

    currentFrame = 0;
    timer = 0.0f;
    playing = 0;
    generated = frameCount > 0;

    snprintf(status, sizeof(status), "%s ready.",
             algorithmNames[selectedAlgorithm]);
}

static void DrawButton(Rectangle rect, const char *label, int selected)
{
    Color fill = selected ? SKYBLUE :
                 (MouseOver(rect) ? LIGHTGRAY : RAYWHITE);

    DrawRectangleRounded(rect, 0.15f, 6, fill);
    DrawRectangleRoundedLines(rect, 0.15f, 6, DARKGRAY);

    int width = MeasureText(label, 13);

    DrawText(label,
             (int)(rect.x + (rect.width - width) / 2),
             (int)rect.y + 12,
             13, BLACK);
}

static void DrawArray(void)
{
    if (!generated || frames == NULL)
        return;

    const SortFrame *frame = &frames[currentFrame];

    float startX = 310;
    float baseY = 550;
    float maxHeight = 280;
    float barWidth = 27;
    float gap = 7;

    for (int i = 0; i < arrayCount; i++) {
        float x = startX + i * (barWidth + gap);
        float height = (frame->values[i] / 100.0f) * maxHeight;

        Color color = BAR_COLOR;

        if (currentFrame == frameCount - 1)
            color = SORTED_COLOR;
        else if (i == frame->activeA || i == frame->activeB)
            color = ACTIVE_COLOR;

        DrawRectangle((int)x,
                      (int)(baseY - height),
                      (int)barWidth,
                      (int)height,
                      color);

        DrawRectangleLines((int)x,
                           (int)(baseY - height),
                           (int)barWidth,
                           (int)height,
                           DARKBLUE);

        char value[8];
        snprintf(value, sizeof(value), "%d", frame->values[i]);

        int width = MeasureText(value, 12);

        DrawText(value,
                 (int)(x + (barWidth - width) / 2),
                 (int)(baseY + 8),
                 12, DARKGRAY);
    }

    DrawLine(295, (int)baseY, 1140, (int)baseY, GRAY);
}

static void DrawStatistics(void)
{
    if (!generated || frames == NULL)
        return;

    const SortFrame *frame = &frames[currentFrame];

    char text[120];

    snprintf(text, sizeof(text), "Comparisons: %d",
             frame->comparisons);
    DrawText(text, 285, 620, 17, DARKBLUE);

    snprintf(text, sizeof(text), "Writes / swaps: %d",
             frame->writes);
    DrawText(text, 480, 620, 17, DARKBLUE);

    snprintf(text, sizeof(text), "Frame: %d / %d",
             currentFrame + 1, frameCount);
    DrawText(text, 720, 620, 17, DARKBLUE);
}

void SortingVisualizerInit(void)
{
    arrayCount = ARRAY_SIZE;
    selectedAlgorithm = SORT_BUBBLE;

    generated = 0;
    playing = 0;
    wantsDashboard = 0; // Ensure wantsDashboard is cleared on init

    frames = NULL;
    frameCount = 0;
    currentFrame = 0;

    GenerateRandomArray();
}

void SortingVisualizerUpdate(void)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        wantsDashboard = 1;
        playing = 0;
        return;
    }

    if (playing && generated) {
        timer += GetFrameTime();

        if (timer >= frameDelay) {
            timer = 0.0f;

            if (currentFrame < frameCount - 1)
                currentFrame++;
            else {
                playing = 0;
                snprintf(status, sizeof(status), "Sorting completed!");
            }
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
        MouseOver(speedSlider)) {
        float mouseX = GetMousePosition().x;

        float ratio = (mouseX - speedSlider.x) /
                      speedSlider.width;

        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;

        frameDelay = 0.35f - ratio * 0.32f;
    }

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return;

    if (MouseOver(generateButton)) {
        GenerateRandomArray();
    }
    else if (MouseOver(startButton)) {
        if (!generated) {
            BuildSortingFrames();
        }

        if (currentFrame >= frameCount - 1)
            currentFrame = 0;

        playing = !playing;
        snprintf(status, sizeof(status),
                 playing ? "Animation playing." : "Animation paused.");
    }
    else if (MouseOver(resetButton)) {
        playing = 0;
        currentFrame = 0;
        timer = 0.0f;
        snprintf(status, sizeof(status), "Animation reset.");
    }
    else {
        for (int i = 0; i < 5; i++) {
            if (MouseOver(algorithmButtons[i])) {
                selectedAlgorithm = (SortAlgorithm)i;
                generated = 0;
                playing = 0;
                currentFrame = 0;

                snprintf(status, sizeof(status),
                         "%s selected. Click START.",
                         algorithmNames[i]);
                break;
            }
        }
    }
}

void SortingVisualizerDraw(void)
{
    DrawText("SORTING ALGORITHMS VISUALIZER",
             285, 18, 25, DARKBLUE);

    for (int i = 0; i < 5; i++)
        DrawButton(algorithmButtons[i], algorithmNames[i],
                   (int)selectedAlgorithm == i);

    DrawButton(generateButton, "NEW ARRAY", 0);
    DrawButton(startButton, playing ? "PAUSE" : "START / RESUME", 0);
    DrawButton(resetButton, "RESET", 0);

    DrawText("Speed", 675, 110, 15, DARKGRAY);
    DrawRectangleRec(speedSlider, LIGHTGRAY);
    DrawCircle((int)(speedSlider.x +
                     (0.35f - frameDelay) / 0.32f *
                     speedSlider.width),
               (int)(speedSlider.y + 6), 9, BLUE);

    DrawText(status, 285, 165, 17, DARKGREEN);

    DrawText("ARRAY VISUALIZATION", 285, 210, 18, DARKBLUE);

    if (!generated) {
        DrawText("Click START to generate sorting animation frames.",
                 285, 300, 18, GRAY);
    } else {
        DrawArray();
        DrawStatistics();
    }

    DrawText("Orange: active elements    Cyan: unsorted    Green: sorted",
             285, 680, 14, GRAY);
}

int SortingVisualizerWantsDashboard(void)
{
    int ret = wantsDashboard;
    wantsDashboard = 0; // Reset flag immediately after it's checked so it doesn't lock up re-entry
    return ret;
}