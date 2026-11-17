#include "binary_tree_visualizer.h"
#include "tree_view.h"
#include "../structures/binary_tree.h"

#include <stdio.h>

#define BST_LIMIT 40

static BinarySearchTree tree;
static TVState tv;
static TVNode nodes[TV_MAX];
static char buf[8];
static int active = 1;
static UIStatus st;

void BSTVisualizerInit(void)
{
    BSTClear(&tree);
    BSTInit(&tree);
    TVReset(&tv);
    buf[0] = '\0';
    active = 1;
    UISetStatus(&st, "Binary Search Tree ready. Insert some values.", C_CYAN);
}

static int Flatten(const TreeNode *t, int *n)
{
    if (t == NULL || *n >= TV_MAX)
        return -1;

    int i = (*n)++;
    nodes[i].value = t->data;
    nodes[i].balance = 0;
    int l = Flatten(t->left, n);
    int r = Flatten(t->right, n);
    nodes[i].left = l;
    nodes[i].right = r;
    return i;
}

static int PathTo(int value, int *path, int *found)
{
    const TreeNode *c = tree.root;
    int n = 0;

    *found = 0;

    while (c != NULL && n < TV_MAX)
    {
        path[n++] = c->data;

        if (value == c->data)
        {
            *found = 1;
            break;
        }

        c = value < c->data ? c->left : c->right;
    }

    return n;
}

static void DoInsert(void)
{
    int v, path[TV_MAX], found, n;
    char msg[120];

    if (!UIParseInt(buf, &v))
    {
        UISetStatus(&st, "Enter a valid integer.", C_RED);
        return;
    }

    if (tree.size >= BST_LIMIT)
    {
        UISetStatus(&st, "Display limit reached (40 nodes).", C_AMBER);
        return;
    }

    if (BSTInsert(&tree, v))
    {
        n = PathTo(v, path, &found);
        TVSetOrder(&tv, path, n, 1);
        snprintf(msg, sizeof(msg), "Inserted %d after %d comparison(s).", v, n);
        UISetStatus(&st, msg, C_GREEN);
        buf[0] = '\0';
    }
    else
        UISetStatus(&st, "Duplicate value (BST keeps unique keys).", C_RED);
}

static void ShowTraversal(int type)
{
    int out[TV_MAX];
    int n = type == 0 ? BSTInorder(&tree, out, TV_MAX)
                      : (type == 1 ? BSTPreorder(&tree, out, TV_MAX) : BSTPostorder(&tree, out, TV_MAX));

    if (n == 0)
    {
        UISetStatus(&st, "The tree is empty.", C_RED);
        return;
    }

    char msg[200];
    int pos = snprintf(msg, sizeof(msg), "%s:  ", type == 0 ? "INORDER" : (type == 1 ? "PREORDER" : "POSTORDER"));

    for (int i = 0; i < n && pos < (int)sizeof(msg) - 8; i++)
        pos += snprintf(msg + pos, sizeof(msg) - (size_t)pos, "%d ", out[i]);

    TVSetOrder(&tv, out, n, -1);
    UISetStatus(&st, msg, C_PURPLE);
}

void BSTVisualizerDraw(void)
{
    int v;

    UIBackground();
    UIHeader("BINARY SEARCH TREE", "Left < node < right   |   animated search paths and traversals", C_CYAN);

    UICard((Rectangle){30, 100, 890, 150}, "OPERATIONS", C_CYAN);

    if (UIInput((Rectangle){50, 135, 170, 46}, buf, sizeof(buf), &active, "Enter value", 0))
        DoInsert();

    if (UIButton((Rectangle){235, 135, 105, 46}, "INSERT", C_GREEN))
        DoInsert();

    if (UIButton((Rectangle){350, 135, 105, 46}, "SEARCH", C_BLUE))
    {
        if (UIParseInt(buf, &v))
        {
            int path[TV_MAX], found;
            int n = PathTo(v, path, &found);
            char msg[120];

            TVSetOrder(&tv, path, n, found);

            if (found)
                snprintf(msg, sizeof(msg), "Found %d after %d comparison(s).", v, n);
            else
                snprintf(msg, sizeof(msg), "%d not found (%d comparison(s)).", v, n);

            UISetStatus(&st, msg, found ? C_GREEN : C_RED);
        }
        else
            UISetStatus(&st, "Enter a valid integer.", C_RED);
    }

    if (UIButton((Rectangle){465, 135, 105, 46}, "DELETE", C_RED))
    {
        if (UIParseInt(buf, &v))
        {
            if (BSTDelete(&tree, v))
            {
                TVSetOrder(&tv, NULL, 0, -1);
                UISetStatus(&st, "Node deleted (two-child case uses the inorder successor).", C_GREEN);
                buf[0] = '\0';
            }
            else
                UISetStatus(&st, "Value not found.", C_RED);
        }
        else
            UISetStatus(&st, "Enter a valid integer.", C_RED);
    }

    if (UIButton((Rectangle){580, 135, 125, 46}, "ADD 5 RANDOM", C_PURPLE))
    {
        int added = 0;

        for (int i = 0; i < 5 && tree.size < BST_LIMIT; i++)
            added += BSTInsert(&tree, GetRandomValue(1, 99));

        TVSetOrder(&tv, NULL, 0, -1);
        UISetStatus(&st, TextFormat("Inserted %d random value(s).", added), C_GREEN);
    }

    if (UIButton((Rectangle){715, 135, 100, 46}, "CLEAR", C_RED))
    {
        BSTClear(&tree);
        TVSetOrder(&tv, NULL, 0, -1);
        UISetStatus(&st, "Tree cleared.", C_AMBER);
    }

    if (UIButton((Rectangle){50, 197, 125, 40}, "INORDER", C_CYAN))
        ShowTraversal(0);
    if (UIButton((Rectangle){185, 197, 125, 40}, "PREORDER", C_GREEN))
        ShowTraversal(1);
    if (UIButton((Rectangle){320, 197, 135, 40}, "POSTORDER", C_PURPLE))
        ShowTraversal(2);

    if (UIButton((Rectangle){480, 197, 110, 40}, "MINIMUM", C_BLUE))
    {
        if (BSTMinimum(&tree, &v))
        {
            int p[1] = {v};
            TVSetOrder(&tv, p, 1, 1);
            UISetStatus(&st, TextFormat("Minimum value: %d (leftmost node).", v), C_GREEN);
        }
        else
            UISetStatus(&st, "The tree is empty.", C_RED);
    }

    if (UIButton((Rectangle){600, 197, 110, 40}, "MAXIMUM", C_BLUE))
    {
        if (BSTMaximum(&tree, &v))
        {
            int p[1] = {v};
            TVSetOrder(&tv, p, 1, 1);
            UISetStatus(&st, TextFormat("Maximum value: %d (rightmost node).", v), C_GREEN);
        }
        else
            UISetStatus(&st, "The tree is empty.", C_RED);
    }

    Rectangle canvas = {30, 265, 890, 380};
    UICard(canvas, "TREE VIEW", C_CYAN);
    DrawText(TextFormat("NODES  %d", tree.size), 800, 278, 14, C_CYAN);

    int count = 0;
    Flatten(tree.root, &count);
    TVDraw(&tv, nodes, count, canvas, 0);

    UIStatusBar(&st, (Rectangle){30, 655, 890, 80});
}
