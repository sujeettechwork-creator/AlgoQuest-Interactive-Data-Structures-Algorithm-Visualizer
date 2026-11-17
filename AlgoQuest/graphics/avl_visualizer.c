#include "avl_visualizer.h"
#include "tree_view.h"
#include "../structures/avl_tree.h"

#include <stdio.h>

#define AVL_LIMIT 40

static AVLTree tree;
static TVState tv;
static TVNode nodes[TV_MAX];
static char buf[8];
static int active = 1;
static UIStatus st;

void AVLVisualizerInit(void)
{
    AVLClear(&tree);
    AVLInit(&tree);
    TVReset(&tv);
    buf[0] = '\0';
    active = 1;
    UISetStatus(&st, "AVL tree ready. Try inserting 10, 20, 30 to see a rotation.", C_PURPLE);
}

static int H(const AVLNode *n) { return n ? n->height : 0; }

static int Flatten(const AVLNode *t, int *n)
{
    if (t == NULL || *n >= TV_MAX)
        return -1;

    int i = (*n)++;
    nodes[i].value = t->data;
    nodes[i].balance = H(t->left) - H(t->right);
    int l = Flatten(t->left, n);
    int r = Flatten(t->right, n);
    nodes[i].left = l;
    nodes[i].right = r;
    return i;
}

static int RootValue(void) { return tree.root ? tree.root->data : 0; }

static void DoInsert(void)
{
    int v;
    char msg[140];

    if (!UIParseInt(buf, &v))
    {
        UISetStatus(&st, "Enter a valid integer.", C_RED);
        return;
    }

    if (tree.size >= AVL_LIMIT)
    {
        UISetStatus(&st, "Display limit reached (40 nodes).", C_AMBER);
        return;
    }

    int oldRoot = RootValue();

    if (AVLInsert(&tree, v))
    {
        int p[1] = {v};
        TVSetOrder(&tv, p, 1, 1);

        if (tree.size > 1 && RootValue() != oldRoot)
            snprintf(msg, sizeof(msg), "Inserted %d. The root changed (%d -> %d): a rotation rebalanced the tree.",
                     v, oldRoot, RootValue());
        else
            snprintf(msg, sizeof(msg), "Inserted %d. Balance factors updated (rotations applied if needed).", v);

        UISetStatus(&st, msg, C_GREEN);
        buf[0] = '\0';
    }
    else
        UISetStatus(&st, "Duplicate value (AVL keeps unique keys).", C_RED);
}

static void ShowTraversal(int type)
{
    int out[TV_MAX];
    int n = type == 0 ? AVLInorder(&tree, out, TV_MAX)
                      : (type == 1 ? AVLPreorder(&tree, out, TV_MAX) : AVLPostorder(&tree, out, TV_MAX));

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

void AVLVisualizerDraw(void)
{
    int v;

    UIBackground();
    UIHeader("AVL TREE VISUALIZER", "Self-balancing BST   |   balance factor = height(left) - height(right)", C_PURPLE);

    UICard((Rectangle){30, 100, 890, 150}, "OPERATIONS", C_PURPLE);

    if (UIInput((Rectangle){50, 135, 170, 46}, buf, sizeof(buf), &active, "Enter value", 0))
        DoInsert();

    if (UIButton((Rectangle){235, 135, 105, 46}, "INSERT", C_GREEN))
        DoInsert();

    if (UIButton((Rectangle){350, 135, 105, 46}, "SEARCH", C_BLUE))
    {
        if (UIParseInt(buf, &v))
        {
            int found = AVLSearch(&tree, v);
            int path[TV_MAX], n = 0;
            const AVLNode *c = tree.root;

            while (c != NULL && n < TV_MAX)
            {
                path[n++] = c->data;
                if (c->data == v)
                    break;
                c = v < c->data ? c->left : c->right;
            }

            TVSetOrder(&tv, path, n, found);
            UISetStatus(&st, found ? TextFormat("Found %d after %d comparison(s).", v, n)
                                   : TextFormat("%d not found (%d comparison(s)).", v, n),
                        found ? C_GREEN : C_RED);
        }
        else
            UISetStatus(&st, "Enter a valid integer.", C_RED);
    }

    if (UIButton((Rectangle){465, 135, 105, 46}, "DELETE", C_RED))
    {
        if (UIParseInt(buf, &v))
        {
            if (AVLDelete(&tree, v))
            {
                TVSetOrder(&tv, NULL, 0, -1);
                UISetStatus(&st, "Deleted. Ancestors were rebalanced on the way back up.", C_GREEN);
                buf[0] = '\0';
            }
            else
                UISetStatus(&st, "Value not found.", C_RED);
        }
        else
            UISetStatus(&st, "Enter a valid integer.", C_RED);
    }

    if (UIButton((Rectangle){580, 135, 125, 46}, "ADD 5 RANDOM", C_CYAN))
    {
        int added = 0;

        for (int i = 0; i < 5 && tree.size < AVL_LIMIT; i++)
            added += AVLInsert(&tree, GetRandomValue(1, 99));

        TVSetOrder(&tv, NULL, 0, -1);
        UISetStatus(&st, TextFormat("Inserted %d random value(s).", added), C_GREEN);
    }

    if (UIButton((Rectangle){715, 135, 100, 46}, "CLEAR", C_RED))
    {
        AVLClear(&tree);
        TVSetOrder(&tv, NULL, 0, -1);
        UISetStatus(&st, "Tree cleared.", C_AMBER);
    }

    if (UIButton((Rectangle){50, 197, 125, 40}, "INORDER", C_CYAN))
        ShowTraversal(0);
    if (UIButton((Rectangle){185, 197, 125, 40}, "PREORDER", C_GREEN))
        ShowTraversal(1);
    if (UIButton((Rectangle){320, 197, 135, 40}, "POSTORDER", C_PURPLE))
        ShowTraversal(2);

    DrawText("Green bf = perfectly balanced, amber bf = leaning by one. Every node stays within +/- 1.",
             480, 208, 13, C_MUTED);

    Rectangle canvas = {30, 265, 890, 380};
    UICard(canvas, "TREE VIEW", C_PURPLE);
    DrawText(TextFormat("NODES  %d     HEIGHT  %d", tree.size, H(tree.root)), 700, 278, 14, C_PURPLE);

    int count = 0;
    Flatten(tree.root, &count);
    TVDraw(&tv, nodes, count, canvas, 1);

    UIStatusBar(&st, (Rectangle){30, 655, 890, 80});
}
