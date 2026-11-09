#ifndef BINARY_TREE_H
#define BINARY_TREE_H

typedef struct TreeNode
{
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

typedef struct
{
    TreeNode *root;
    int size;
} BinarySearchTree;

void BSTInit(BinarySearchTree *tree);
int BSTInsert(BinarySearchTree *tree, int value);
int BSTSearch(const BinarySearchTree *tree, int value);
int BSTDelete(BinarySearchTree *tree, int value);
int BSTMinimum(const BinarySearchTree *tree, int *value);
int BSTMaximum(const BinarySearchTree *tree, int *value);
void BSTClear(BinarySearchTree *tree);

int BSTInorder(const BinarySearchTree *tree, int output[], int capacity);
int BSTPreorder(const BinarySearchTree *tree, int output[], int capacity);
int BSTPostorder(const BinarySearchTree *tree, int output[], int capacity);

#endif