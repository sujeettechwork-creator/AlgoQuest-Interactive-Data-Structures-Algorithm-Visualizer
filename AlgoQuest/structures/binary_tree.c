#include "binary_tree.h"
#include <stdlib.h>

static TreeNode *CreateNode(int value)
{
    TreeNode *node = malloc(sizeof(TreeNode));

    if (node == NULL)
        return NULL;

    node->data = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void BSTInit(BinarySearchTree *tree)
{
    tree->root = NULL;
    tree->size = 0;
}

int BSTInsert(BinarySearchTree *tree, int value)
{
    TreeNode **current = &tree->root;

    while (*current != NULL)
    {
        if (value == (*current)->data)
            return 0;

        if (value < (*current)->data)
            current = &(*current)->left;
        else
            current = &(*current)->right;
    }

    *current = CreateNode(value);

    if (*current == NULL)
        return 0;

    tree->size++;
    return 1;
}

int BSTSearch(const BinarySearchTree *tree, int value)
{
    TreeNode *current = tree->root;

    while (current != NULL)
    {
        if (value == current->data)
            return 1;

        if (value < current->data)
            current = current->left;
        else
            current = current->right;
    }

    return 0;
}

static TreeNode *FindMinimumNode(TreeNode *node)
{
    while (node != NULL && node->left != NULL)
        node = node->left;

    return node;
}

static TreeNode *DeleteNode(TreeNode *root, int value, int *deleted)
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

        if (root->left == NULL)
        {
            TreeNode *temp = root->right;
            free(root);
            return temp;
        }

        if (root->right == NULL)
        {
            TreeNode *temp = root->left;
            free(root);
            return temp;
        }

        TreeNode *successor = FindMinimumNode(root->right);
        root->data = successor->data;

        int ignored = 0;
        root->right = DeleteNode(root->right, successor->data, &ignored);
    }

    return root;
}

int BSTDelete(BinarySearchTree *tree, int value)
{
    int deleted = 0;

    tree->root = DeleteNode(tree->root, value, &deleted);

    if (deleted)
        tree->size--;

    return deleted;
}

int BSTMinimum(const BinarySearchTree *tree, int *value)
{
    TreeNode *node = FindMinimumNode(tree->root);

    if (node == NULL)
        return 0;

    *value = node->data;
    return 1;
}

int BSTMaximum(const BinarySearchTree *tree, int *value)
{
    TreeNode *node = tree->root;

    if (node == NULL)
        return 0;

    while (node->right != NULL)
        node = node->right;

    *value = node->data;
    return 1;
}

static void FreeTree(TreeNode *root)
{
    if (root == NULL)
        return;

    FreeTree(root->left);
    FreeTree(root->right);
    free(root);
}

void BSTClear(BinarySearchTree *tree)
{
    FreeTree(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

static void Inorder(TreeNode *root, int output[],
                    int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    Inorder(root->left, output, capacity, count);

    if (*count < capacity)
        output[(*count)++] = root->data;

    Inorder(root->right, output, capacity, count);
}

static void Preorder(TreeNode *root, int output[],
                     int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    output[(*count)++] = root->data;

    Preorder(root->left, output, capacity, count);
    Preorder(root->right, output, capacity, count);
}

static void Postorder(TreeNode *root, int output[],
                      int capacity, int *count)
{
    if (root == NULL || *count >= capacity)
        return;

    Postorder(root->left, output, capacity, count);
    Postorder(root->right, output, capacity, count);

    if (*count < capacity)
        output[(*count)++] = root->data;
}

int BSTInorder(const BinarySearchTree *tree,
               int output[], int capacity)
{
    int count = 0;
    Inorder(tree->root, output, capacity, &count);
    return count;
}

int BSTPreorder(const BinarySearchTree *tree,
                int output[], int capacity)
{
    int count = 0;
    Preorder(tree->root, output, capacity, &count);
    return count;
}

int BSTPostorder(const BinarySearchTree *tree,
                 int output[], int capacity)
{
    int count = 0;
    Postorder(tree->root, output, capacity, &count);
    return count;
}