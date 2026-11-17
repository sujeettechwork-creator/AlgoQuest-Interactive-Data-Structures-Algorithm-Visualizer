#include "sorting_visualizer.h"
#include "ui.h"
#include "../structures/sorting.h"

#include <stdio.h>

#define ARRAY_SIZE 25

static int original[ARRAY_SIZE];
static SortAlgorithm algo = SORT_BUBBLE;
static const SortFrame *frames = NULL;
static int frameCount = 0;
static int cur = 0;
static float timer = 0.0f;
static float speed = 0.55f; /* 0..1 */
static int playing = 0;
static float sweep = 0.0f;
static int dragSpeed = 0;
static int dragScrub = 0;
static UIStatus st;

static const char *names[] = {"Bubble Sort", "Selection Sort", "Insertion Sort", "Merge Sort", "Quick Sort"};
static const char *bestCase[] = {"O(n)", "O(n^2)", "O(n)", "O(n log n)", "O(n log n)"};
static const char *avgCase[] = {"O(n^2)", "O(n^2)", "O(n^2)", "O(n log n)", "O(n log n)"};
static const char *worstCase[] = {"O(n^2)", "O(n^2)", "O(n^2)", "O(n log n)", "O(n^2)"};
static const char *spaceCase[] = {"O(1)", "O(1)", "O(1)", "O(n)", "O(log n)"};
static const char *stableCase[] = {"Yes", "No", "Yes", "Yes", "No"};

static void Build(void)
{
    SortGenerate(algo, original, ARRAY_SIZE);
    frames = SortGetFrames();
    frameCount = SortGetFrameCount();
    cur = 0;
    timer = 0.0f;
    playing = 0;
    sweep = 0.0f;
}

static void NewArray(void)
{
    for (int i = 0; i < ARRAY_SIZE; i++)
        original[i] = GetRandomValue(5, 100);

    Build();
    UISetStatus(&st, "New random array generated. Press PLAY.", C_CYAN);
}

void SortingVisualizerInit(void)
{
    algo = SORT_BUBBLE;
    speed = 0.55f;
    NewArray();
}

static float Delay(void) { return 0.40f - speed * 0.385f; }

void SortingVisualizerDraw(void)
{
    UIBackground();
    UIHeader("SORTING ALGORITHMS", "Every swap and write is recorded as a frame you can play, step and scrub", C_CYAN);

    /* algorithm tabs */
    for (int i = 0; i < 5; i++)
        if (UIButton((Rectangle){30.0f + i * 180.0f, 100, 170, 40}, names[i], (int)algo == i ? C_CYAN : C_PANEL2))
        {
            algo = (SortAlgorithm)i;
            Build();
            UISetStatus(&st, TextFormat("%s selected on the same array. Press PLAY.", names[i]), C_CYAN);
        }

    /* controls */
    UICard((Rectangle){30, 150, 890, 62}, NULL, C_CYAN);

    if (UIButton((Rectangle){42, 160, 120, 42}, "NEW ARRAY", C_PURPLE))
        NewArray();

    if (UIButton((Rectangle){172, 160, 110, 42}, playing ? "PAUSE" : "PLAY", C_GREEN))
    {
        if (cur >= frameCount - 1)
        {
            cur = 0;
            sweep = 0.0f;
        }
        playing = !playing;
        UISetStatus(&st, playing ? "Playing animation." : "Paused.", playing ? C_GREEN : C_AMBER);
    }

    if (UIButton((Rectangle){292, 160, 70, 42}, "< STEP", C_BLUE))
    {
        if (cur > 0) cur--;
        playing = 0;
        sweep = 0.0f;
    }

    if (UIButton((Rectangle){370, 160, 70, 42}, "STEP >", C_BLUE))
    {
        if (cur < frameCount - 1) cur++;
        playing = 0;
    }

    if (UIButton((Rectangle){448, 160, 90, 42}, "RESET", C_RED))
    {
        cur = 0;
        playing = 0;
        timer = 0.0f;
        sweep = 0.0f;
        UISetStatus(&st, "Animation reset to the first frame.", C_AMBER);
    }

    /* speed slider */
    Rectangle track = {640, 178, 240, 8};
    Rectangle hit = {track.x - 12, track.y - 14, track.width + 24, track.height + 28};
    DrawText("SPEED", 580, 173, 14, C_MUTED);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), hit))
        dragSpeed = 1;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        dragSpeed = 0;
    if (dragSpeed)
        speed = UIClampf((GetMousePosition().x - track.x) / track.width, 0.0f, 1.0f);

    DrawRectangleRounded(track, 0.5f, 6, C_BG);
    DrawRectangleRounded((Rectangle){track.x, track.y, track.width * speed, track.height}, 0.5f, 6, C_CYAN);
    DrawCircleV((Vector2){track.x + track.width * speed, track.y + 4}, 9, Fade(C_CYAN, 0.35f));
    DrawCircleV((Vector2){track.x + track.width * speed, track.y + 4}, 6, C_WHITE);

    /* playback */
    if (playing)
    {
        timer += GetFrameTime();

        while (timer >= Delay() && playing)
        {
            timer -= Delay();

            if (cur < frameCount - 1)
                cur++;
            else
            {
                playing = 0;
                UISetStatus(&st, "Sorting completed!", C_GREEN);
            }
        }
    }

    if (cur >= frameCount - 1)
        sweep += GetFrameTime() * 50.0f;

    const SortFrame *f = &frames[cur];

    /* bars */
    Rectangle area = {30, 225, 890, 340};
    UICard(area, "ARRAY", C_CYAN);

    DrawRectangle(560, 232, 10, 10, C_AMBER);
    DrawText("active", 576, 230, 13, C_MUTED);
    DrawRectangle(640, 232, 10, 10, C_PURPLE);
    DrawText("pivot / range end", 656, 230, 13, C_MUTED);
    DrawRectangle(790, 232, 10, 10, C_GREEN);
    DrawText("sorted", 806, 230, 13, C_MUTED);

    float baseY = 545.0f, maxH = 270.0f, gap = 6.0f;
    float barW = (area.width - 60.0f - (ARRAY_SIZE - 1) * gap) / ARRAY_SIZE;

    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        float x = area.x + 30.0f + i * (barW + gap);
        float h = f->values[i] / 100.0f * maxH;
        Color c = C_CYAN;

        if (cur >= frameCount - 1)
            c = (float)i < sweep ? C_GREEN : C_CYAN;
        else if (i == f->activeA)
            c = C_AMBER;
        else if (i == f->activeB)
            c = C_PURPLE;

        if (c.r == C_AMBER.r || c.r == C_PURPLE.r)
            DrawRectangleRounded((Rectangle){x - 3, baseY - h - 3, barW + 6, h + 6}, 0.25f, 6, Fade(c, 0.25f));

        DrawRectangleGradientV((int)x, (int)(baseY - h), (int)barW, (int)h, c, Fade(c, 0.45f));
        DrawRectangleRoundedLinesEx((Rectangle){x, baseY - h, barW, h}, 0.2f, 4, 1.0f, Fade(WHITE, 0.25f));
        DrawText(TextFormat("%d", f->values[i]), (int)(x + barW / 2 - MeasureText(TextFormat("%d", f->values[i]), 11) / 2.0f),
                 (int)baseY + 6, 11, C_MUTED);
    }

    DrawLine((int)area.x + 20, (int)baseY, (int)(area.x + area.width - 20), (int)baseY, Fade(C_MUTED, 0.5f));

    /* stats + scrubber */
    UICard((Rectangle){30, 580, 440, 150}, "LIVE STATISTICS", C_CYAN);
    DrawText(TextFormat("Comparisons: %d", f->comparisons), 56, 612, 18, C_WHITE);
    DrawText(TextFormat("Writes / swaps: %d", f->writes), 56, 640, 18, C_WHITE);
    DrawText(TextFormat("Frame: %d / %d", cur + 1, frameCount), 56, 668, 18, C_CYAN);

    Rectangle bar = {56, 702, 388, 10};
    Rectangle barHit = {bar.x, bar.y - 10, bar.width, bar.height + 20};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), barHit))
        dragScrub = 1;
    if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        dragScrub = 0;
    if (dragScrub && frameCount > 1)
    {
        cur = (int)(UIClampf((GetMousePosition().x - bar.x) / bar.width, 0.0f, 1.0f) * (frameCount - 1) + 0.5f);
        playing = 0;
        sweep = 0.0f;
    }

    DrawRectangleRounded(bar, 0.5f, 6, C_BG);
    if (frameCount > 1)
        DrawRectangleRounded((Rectangle){bar.x, bar.y, bar.width * cur / (frameCount - 1), bar.height}, 0.5f, 6, C_GREEN);

    UICard((Rectangle){480, 580, 440, 150}, "COMPLEXITY", C_PURPLE);
    DrawText(names[algo], 506, 608, 17, C_PURPLE);
    DrawText(TextFormat("Best: %s     Average: %s     Worst: %s", bestCase[algo], avgCase[algo], worstCase[algo]), 506, 640, 14, C_WHITE);
    DrawText(TextFormat("Extra space: %s     Stable: %s", spaceCase[algo], stableCase[algo]), 506, 664, 14, C_WHITE);

    DrawText(st.text, 506, 696, 15, st.color);
}
