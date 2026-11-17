#include "challenge_visualizer.h"
#include "ui.h"

#include <math.h>
#include <stdio.h>

typedef struct
{
    const char *q;
    const char *o[4];
    int answer;
} Question;

static const Question bank[] = {
    {"Which data structure follows LIFO order?", {"Queue", "Stack", "Heap", "Graph"}, 1},
    {"Which data structure follows FIFO order?", {"Queue", "Stack", "Binary tree", "Heap"}, 0},
    {"Time complexity of binary search?", {"O(n)", "O(log n)", "O(n log n)", "O(1)"}, 1},
    {"Worst-case time of Quick Sort?", {"O(n log n)", "O(n)", "O(n^2)", "O(log n)"}, 2},
    {"Which BST traversal prints keys in sorted order?", {"Preorder", "Postorder", "Inorder", "Level order"}, 2},
    {"Max |balance factor| allowed in an AVL tree?", {"0", "1", "2", "3"}, 1},
    {"What does the root of a max-heap hold?", {"Smallest key", "Largest key", "Median", "Last inserted"}, 1},
    {"BFS uses which helper structure?", {"Stack", "Queue", "Heap", "Hash table"}, 1},
    {"DFS naturally uses which structure?", {"Queue", "Stack / recursion", "Heap", "Deque only"}, 1},
    {"Insert at the head of a singly linked list?", {"O(1)", "O(n)", "O(log n)", "O(n^2)"}, 0},
    {"Which sort is stable AND O(n log n) worst case?", {"Quick Sort", "Heap Sort", "Merge Sort", "Selection Sort"}, 2},
    {"Circular queue index update rule?", {"rear + 1", "(rear + 1) % N", "rear * 2", "N - rear"}, 1},
    {"Stack overflow happens when you...", {"Pop from empty", "Push to full", "Peek empty", "Clear it"}, 1},
    {"Height of a balanced BST with n nodes?", {"O(n)", "O(log n)", "O(1)", "O(n^2)"}, 1},
    {"A doubly linked node stores...", {"Only next", "Next and prev", "Only data", "An index"}, 1},
    {"Bubble sort best case with early exit?", {"O(n^2)", "O(n log n)", "O(n)", "O(1)"}, 2},
    {"Adjacency matrix space for V vertices?", {"O(V)", "O(V + E)", "O(V^2)", "O(E)"}, 2},
    {"Parent of index i in a 0-based array heap?", {"i / 2", "(i - 1) / 2", "2i + 1", "i - 1"}, 1},
    {"Selection sort performs at most how many swaps?", {"n - 1", "n^2", "log n", "2n"}, 0},
    {"Which rotation fixes a Left-Left imbalance?", {"Left rotation", "Right rotation", "Double rotation", "None"}, 1},
    {"Deleting a BST node with two children uses...", {"Inorder successor", "Preorder parent", "Level order", "A random leaf"}, 0},
    {"Which of these is NOT a linear structure?", {"Stack", "Queue", "Linked list", "Binary tree"}, 3},
    {"A priority queue is typically built with a...", {"Stack", "Heap", "Array of lists", "Hash table"}, 1},
    {"Building a heap from n items bottom-up costs...", {"O(n)", "O(n log n)", "O(log n)", "O(n^2)"}, 0}};

#define BANK_SIZE ((int)(sizeof(bank) / sizeof(bank[0])))
#define QUIZ_LEN 10
#define QTIME 15.0f

static int order[BANK_SIZE];
static int phase = 0; /* 0 intro, 1 playing, 2 result */
static int qi = 0, score = 0, streak = 0, bestStreak = 0, correct = 0;
static int chosen = -1; /* -1 waiting, -2 timed out, 0..3 selected */
static float timeLeft = QTIME, revealT = 0.0f;
static int lastGain = 0;

void ChallengeVisualizerInit(void)
{
    phase = 0;
    score = streak = bestStreak = correct = 0;
}

static void StartRound(void)
{
    for (int i = 0; i < BANK_SIZE; i++)
        order[i] = i;

    for (int i = BANK_SIZE - 1; i > 0; i--)
    {
        int j = GetRandomValue(0, i);
        int t = order[i];
        order[i] = order[j];
        order[j] = t;
    }

    qi = 0;
    score = streak = bestStreak = correct = 0;
    chosen = -1;
    timeLeft = QTIME;
    revealT = 0.0f;
    phase = 1;
}

static void Answer(int c)
{
    const Question *q = &bank[order[qi]];

    chosen = c;
    revealT = 0.0f;
    lastGain = 0;

    if (c == q->answer)
    {
        correct++;
        streak++;
        if (streak > bestStreak)
            bestStreak = streak;

        lastGain = 100 + (int)(timeLeft * 10.0f) + (streak - 1) * 25;
        score += lastGain;
    }
    else
        streak = 0;
}

static const char *Rank(void)
{
    if (score >= 1800) return "ALGORITHM GRANDMASTER";
    if (score >= 1300) return "DATA STRUCTURE WIZARD";
    if (score >= 800) return "RISING CODER";
    return "KEEP PRACTISING";
}

void ChallengeVisualizerDraw(void)
{
    UIBackground();
    UIHeader("ALGOQUEST CHALLENGE", "Ten timed questions on everything in this app", C_AMBER);

    if (phase == 0)
    {
        UICard((Rectangle){120, 150, 710, 420}, NULL, C_AMBER);

        const char *t = "READY TO TEST YOURSELF?";
        DrawText(t, CW / 2 - MeasureText(t, 36) / 2, 200, 36, C_WHITE);

        DrawText("10 random questions  |  15 seconds each", 320, 270, 20, C_CYAN);
        DrawText("Points = 100 + time bonus + streak bonus", 305, 305, 20, C_MUTED);
        DrawText("Topics: stacks, queues, lists, trees, heaps, graphs, sorting", 235, 340, 18, C_MUTED);

        float pulse = 0.5f + 0.5f * sinf(UITime() * 3.0f);
        DrawRectangleRounded((Rectangle){338, 408, 274, 74}, 0.3f, 8, Fade(C_AMBER, 0.12f + 0.12f * pulse));

        if (UIButton((Rectangle){350, 420, 250, 50}, "START CHALLENGE", C_AMBER))
            StartRound();

        return;
    }

    if (phase == 2)
    {
        UICard((Rectangle){120, 130, 710, 500}, NULL, C_AMBER);

        const char *t = "CHALLENGE COMPLETE";
        DrawText(t, CW / 2 - MeasureText(t, 34) / 2, 170, 34, C_WHITE);

        const char *r = Rank();
        DrawText(r, CW / 2 - MeasureText(r, 28) / 2, 230, 28, C_AMBER);

        DrawText(TextFormat("%d", score), CW / 2 - MeasureText(TextFormat("%d", score), 80) / 2, 285, 80, C_CYAN);
        DrawText("POINTS", CW / 2 - MeasureText("POINTS", 16) / 2, 372, 16, C_MUTED);

        DrawText(TextFormat("Correct: %d / %d", correct, QUIZ_LEN), 330, 420, 22, C_GREEN);
        DrawText(TextFormat("Accuracy: %d%%", correct * 100 / QUIZ_LEN), 330, 455, 22, C_WHITE);
        DrawText(TextFormat("Best streak: %d", bestStreak), 330, 490, 22, C_PURPLE);

        if (UIButton((Rectangle){350, 545, 250, 50}, "PLAY AGAIN", C_AMBER))
            StartRound();

        return;
    }

    /* ---- question phase ---- */
    const Question *q = &bank[order[qi]];

    if (chosen == -1)
    {
        timeLeft -= GetFrameTime();
        if (timeLeft <= 0.0f)
        {
            timeLeft = 0.0f;
            Answer(-2);
        }
    }
    else
    {
        revealT += GetFrameTime();
        if (revealT > 2.0f)
        {
            qi++;
            if (qi >= QUIZ_LEN)
            {
                phase = 2;
                return;
            }
            chosen = -1;
            timeLeft = QTIME;
        }
    }

    UICard((Rectangle){30, 100, 890, 150}, TextFormat("QUESTION %d / %d", qi + 1, QUIZ_LEN), C_AMBER);

    DrawText(q->q, 56, 150, 26, C_WHITE);
    DrawText(TextFormat("SCORE  %d", score), 56, 205, 18, C_CYAN);
    DrawText(TextFormat("STREAK  %d", streak), 220, 205, 18, C_PURPLE);

    /* timer ring */
    float frac = timeLeft / QTIME;
    Vector2 c = {850, 175};
    Color tc = frac > 0.5f ? C_GREEN : (frac > 0.25f ? C_AMBER : C_RED);

    DrawRing(c, 30, 36, 0, 360, 48, Fade(C_BG, 0.9f));
    DrawRing(c, 30, 36, -90, -90 + 360 * frac, 48, tc);
    DrawText(TextFormat("%d", (int)(timeLeft + 0.99f)), (int)c.x - MeasureText(TextFormat("%d", (int)(timeLeft + 0.99f)), 24) / 2, (int)c.y - 12, 24, C_WHITE);

    /* progress bar */
    DrawRectangleRounded((Rectangle){30, 262, 890, 8}, 0.5f, 6, C_PANEL);
    DrawRectangleRounded((Rectangle){30, 262, 890.0f * (float)qi / QUIZ_LEN, 8}, 0.5f, 6, C_AMBER);

    static const char *letters[] = {"A", "B", "C", "D"};

    for (int i = 0; i < 4; i++)
    {
        Rectangle r = {30, 290.0f + i * 78.0f, 890, 64};
        Color base = (Color){38, 50, 88, 255};
        char label[100];

        snprintf(label, sizeof(label), "%s)   %s", letters[i], q->o[i]);

        if (chosen != -1)
        {
            if (i == q->answer)
                base = C_GREEN;
            else if (i == chosen)
                base = C_RED;
            else
                base = (Color){26, 32, 56, 255};
        }

        if (UIButton(r, label, base) && chosen == -1)
            Answer(i);
    }

    if (chosen != -1)
    {
        const char *msg = chosen == q->answer ? TextFormat("Correct!  +%d points", lastGain)
                                              : (chosen == -2 ? "Time is up!" : "Not quite.");

        DrawText(msg, 30, 612, 24, chosen == q->answer ? C_GREEN : C_RED);

        if (UIButton((Rectangle){770, 600, 150, 46}, qi == QUIZ_LEN - 1 ? "FINISH" : "NEXT >", C_AMBER))
            revealT = 10.0f;
    }
    else
        DrawText("Click an answer before the timer runs out.", 30, 612, 18, C_MUTED);
}
