#include "avl_tree.h"
#include <stdlib.h>

static int Height(AVLNode *node)
{
    return node == NULL ? 0 : node->height;
}

static int Max(int a, int b)
{
    return a > b ? a : b;
}

static AVLNode *CreateNode(int value)
{
    AVLNode *node = malloc(sizeof(AVLNode));

    if (node == NULL)
        return NULL;

    node->data = value;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static int BalanceFactor(AVLNode *node)
{
    return node == NULL ? 0 :
        Height(node->left) - Height(node->right);
}

static AVLNode *RightRotate(AVLNode *y)
{
    AVLNode *x = y->left;
    AVLNode *temp = x->right;

    x->right = y;
    y->left = temp;

    y->height = 1 + Max(Height(y->left), Height(y->right));
    x->height = 1 + Max(Height(x->left), Height(x->right));

    return x;
}

static AVLNode *LeftRotate(AVLNode *x)
{
    AVLNode *y = x->right;
    AVLNode *temp = y->left;

    y->left = x;
    x->right = temp;

    x->height = 1 + Max(Height(x->left), Height(x->right));
    y->height = 1 + Max(Height(y->left), Height(y->right));

    return y;
}

static AVLNode *InsertNode(AVLNode *node, int value, int *inserted)
{
    if (node == NULL)
    {
        AVLNode *newNode = CreateNode(value);

        if (newNode != NULL)
            *inserted = 1;

        return newNode;
    }

    if (value < node->data)
        node->left = InsertNode(node->left, value, inserted);

    else if (value > node->data)
        node->right = InsertNode(node->right, value, inserted);

    else
        return node;

    if (!*inserted)
        return node;

    node->height = 1 + Max(Height(node->left), Height(node->right));

    int balance = BalanceFactor(node);

    if (balance > 1 && value < node->left->data)
        return RightRotate(node);

    if (balance < -1 && value > node->right->data)
        return LeftRotate(node);

    if (balance > 1 && value > node->left->data)
    {
        node->left = LeftRotate(node->left);
        return RightRotate(node);
    }

    if (balance < -1 && value < node->right->data)
    {
        node->right = RightRotate(node->right);
        return LeftRotate(node);
    }

    return node;
}

void AVLInit(AVLTree *tree)
{
    tree->root = NULL;
    tree->size = 0;
}

int AVLInsert(AVLTree *tree, int value)
{
    int inserted = 0;

    tree->root = InsertNode(tree->root, value, &inserted);

    if (inserted)
        tree->size++;

    return inserted;
}

int AVLSearch(const AVLTree *tree, int value)
{
    AVLNode *current = tree->root;

    while (current != NULL)
    {
        if (current->data == value)
            return 1;

        current = value < current->data ?
                  current->left : current->right;
    }

    return 0;
}

static AVLNode *MinimumNode(AVLNode *node)
{
    while (node->left != NULL)
        node = node->left;

    return node;
}

static AVLNode *DeleteNode(AVLNode *root, int value, int *deleted)
{
    if (root == NULL)
        return NULL;

    if (value < root->data)
        root->left = DeleteNode(root->left, value, deleted);

    else if (value > root->data)
        root->right = DeleteNode(root->right, value, deleted);

    else
    {
        *deleted = 1;

        if (root->left == NULL || root->right == NULL)
        {
            AVLNode *child = root->left ?
                             root->left : root->right;

            free(root);
            return child;
        }

        AVLNode *successor = MinimumNode(root->right);
        root->data = successor->data;

        int ignored = 0;
        root->right = DeleteNode(root->right,
                                 successor->data, &ignored);
    }

    root->height = 1 + Max(Height(root->left), Height(root->right));

    int balance = BalanceFactor(root);

    if (balance > 1 && BalanceFactor(root->left) >= 0)
        return RightRotate(root);

    if (balance > 1 && BalanceFactor(root->left) < 0)
    {
        root->left = LeftRotate(root->left);
        return RightRotate(root);
    }

    if (balance < -1 && BalanceFactor(root->right) <= 0)
        return LeftRotate(root);

    if (balance < -1 && BalanceFactor(root->right) > 0)
    {
        root->right = RightRotate(root->right);
        return LeftRotate(root);
    }

    return root;
}

int AVLDelete(AVLTree *tree, int value)
{
    int deleted = 0;

    tree->root = DeleteNode(tree->root, value, &deleted);

    if (deleted)
        tree->size--;

    return deleted;
}

static void FreeAVL(AVLNode *root)
{
    if (root == NULL)
        return;

    FreeAVL(root->left);
    FreeAVL(root->right);
    free(root);
}

void AVLClear(AVLTree *tree)
{
    FreeAVL(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

static void Inorder(AVLNode *root, int output[],
                    int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    Inorder(root->left, output, capacity, count);

    if (*count < capacity)
        output[(*count)++] = root->data;

    Inorder(root->right, output, capacity, count);
}

static void Preorder(AVLNode *root, int output[],
                     int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    output[(*count)++] = root->data;

    Preorder(root->left, output, capacity, count);
    Preorder(root->right, output, capacity, count);
}

static void Postorder(AVLNode *root, int output[],
                      int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    Postorder(root->left, output, capacity, count);
    Postorder(root->right, output, capacity, count);

    if (*count < capacity)
        output[(*count)++] = root->data;
}

int AVLInorder(const AVLTree *tree, int output[], int capacity)
{
    int count = 0;
    Inorder(tree->root, output, capacity, &count);
    return count;
}

int AVLPreorder(const AVLTree *tree, int output[], int capacity)
{
    int count = 0;
    Preorder(tree->root, output, capacity, &count);
    return count;
}

int AVLPostorder(const AVLTree *tree, int output[], int capacity)
{
    int count = 0;
    Postorder(tree->root, output, capacity, &count);
    return count;
}