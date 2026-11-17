#include "tree_view.h"

#include <math.h>
#include <string.h>

static int gCounter;
static int gMaxDepth;

static void Layout(TVNode *n, int i, int depth)
{
    if (i < 0)
        return;

    Layout(n, n[i].left, depth + 1);
    n[i].tx = (float)gCounter++;
    n[i].ty = (float)depth;
    if (depth > gMaxDepth)
        gMaxDepth = depth;
    Layout(n, n[i].right, depth + 1);
}

void TVReset(TVState *s)
{
    memset(s, 0, sizeof(*s));
    s->found = -1;
}

void TVSetOrder(TVState *s, const int *order, int n, int found)
{
    if (n > TV_MAX)
        n = TV_MAX;
    if (n < 0)
        n = 0;

    if (n > 0)
        memcpy(s->order, order, sizeof(int) * (size_t)n);

    s->orderCount = n;
    s->progress = 0.0f;
    s->found = found;
}

static int OrderIndex(const TVState *s, int value)
{
    for (int i = 0; i < s->orderCount; i++)
        if (s->order[i] == value)
            return i;
    return -1;
}

void TVDraw(TVState *s, TVNode *nodes, int count, Rectangle canvas, int showBalance)
{
    if (count <= 0)
    {
        const char *t = "EMPTY TREE  -  insert a value to begin";
        DrawText(t, (int)(canvas.x + canvas.width / 2 - MeasureText(t, 22) / 2.0f),
                 (int)(canvas.y + canvas.height / 2 - 11), 22, Fade(C_MUTED, 0.8f));
        s->orderCount = 0;
        for (int k = 0; k < TV_MAX; k++)
            s->used[k] = 0;
        return;
    }

    gCounter = 0;
    gMaxDepth = 0;
    Layout(nodes, 0, 0);

    float sx = fminf(66.0f, (canvas.width - 70.0f) / (float)count);
    float sy = gMaxDepth > 0 ? fminf(80.0f, (canvas.height - 90.0f) / (float)gMaxDepth) : 0.0f;
    float rad = UIClampf(fminf(sx, sy > 0.0f ? sy : sx) * 0.42f, 9.0f, 24.0f);
    float startX = canvas.x + canvas.width / 2 - (count - 1) * sx / 2;
    float y0 = canvas.y + 48.0f;

    int slot[TV_MAX];
    int parent[TV_MAX];
    int seen[TV_MAX] = {0};

    for (int i = 0; i < count; i++)
        parent[i] = -1;

    for (int i = 0; i < count; i++)
    {
        if (nodes[i].left >= 0) parent[nodes[i].left] = i;
        if (nodes[i].right >= 0) parent[nodes[i].right] = i;
    }

    /* pass 1: reuse the animation slot of nodes that already exist */
    for (int i = 0; i < count; i++)
    {
        slot[i] = -1;

        for (int k = 0; k < TV_MAX; k++)
            if (s->used[k] && !seen[k] && s->value[k] == nodes[i].value)
            {
                slot[i] = k;
                seen[k] = 1;
                break;
            }
    }

    for (int k = 0; k < TV_MAX; k++)
        if (s->used[k] && !seen[k])
            s->used[k] = 0;

    /* pass 2: new nodes sprout from their parent's current position */
    for (int i = 0; i < count; i++)
    {
        if (slot[i] >= 0)
            continue;

        for (int k = 0; k < TV_MAX; k++)
            if (!s->used[k])
            {
                s->used[k] = 1;
                s->value[k] = nodes[i].value;
                seen[k] = 1;
                slot[i] = k;

                if (parent[i] >= 0 && slot[parent[i]] >= 0)
                {
                    s->x[k] = s->x[slot[parent[i]]];
                    s->y[k] = s->y[slot[parent[i]]];
                }
                else
                {
                    s->x[k] = startX + nodes[i].tx * sx;
                    s->y[k] = y0 + nodes[i].ty * sy;
                }
                break;
            }
    }

    for (int i = 0; i < count; i++)
    {
        int k = slot[i];
        if (k < 0)
            continue;
        s->x[k] = UIApproach(s->x[k], startX + nodes[i].tx * sx, 9.0f);
        s->y[k] = UIApproach(s->y[k], y0 + nodes[i].ty * sy, 9.0f);
    }

    if (s->orderCount > 0)
        s->progress += GetFrameTime() * 3.2f;

    int shown = (int)s->progress;
    if (shown > s->orderCount)
        shown = s->orderCount;
    int finished = s->orderCount > 0 && shown == s->orderCount;

    /* edges */
    for (int i = 0; i < count; i++)
    {
        int kids[2] = {nodes[i].left, nodes[i].right};

        for (int c = 0; c < 2; c++)
        {
            if (kids[c] < 0 || slot[i] < 0 || slot[kids[c]] < 0)
                continue;

            Vector2 a = {s->x[slot[i]], s->y[slot[i]]};
            Vector2 b = {s->x[slot[kids[c]]], s->y[slot[kids[c]]]};

            int oi = OrderIndex(s, nodes[i].value);
            int oc = OrderIndex(s, nodes[kids[c]].value);
            int lit = s->found >= 0 && oi >= 0 && oc == oi + 1 && oc < shown;

            DrawLineEx(a, b, lit ? 3.5f : 2.0f, lit ? C_AMBER : (Color){80, 96, 140, 255});
        }
    }

    /* nodes */
    for (int i = 0; i < count; i++)
    {
        int k = slot[i];
        if (k < 0)
            continue;

        int oi = OrderIndex(s, nodes[i].value);
        Color fill = C_PANEL2;
        Color border = C_CYAN;
        float glow = 0.0f;

        if (oi >= 0 && oi < shown)
        {
            fill = (Color){20, 74, 96, 255};

            if (oi == shown - 1)
            {
                fill = (Color){120, 84, 20, 255};
                border = C_AMBER;
                glow = 1.0f;
            }

            if (finished && oi == s->orderCount - 1)
            {
                if (s->found == 1)
                {
                    fill = (Color){24, 108, 70, 255};
                    border = C_GREEN;
                    glow = 1.0f;
                }
                else if (s->found == 0)
                {
                    fill = (Color){120, 36, 48, 255};
                    border = C_RED;
                    glow = 1.0f;
                }
            }
        }

        Vector2 c = {s->x[k], s->y[k]};
        UINode(c, rad, TextFormat("%d", nodes[i].value), fill, border, glow);

        if (showBalance && rad >= 14.0f)
        {
            int bf = nodes[i].balance;
            const char *t = TextFormat("bf %+d", bf);
            DrawText(t, (int)(c.x - MeasureText(t, 11) / 2.0f), (int)(c.y + rad + 4), 11,
                     bf == 0 ? C_GREEN : C_AMBER);
        }
    }
}
