#ifndef AVL_TREE_H
#define AVL_TREE_H

typedef struct AVLNode
{
    int data;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

typedef struct
{
    AVLNode *root;
    int size;
} AVLTree;

void AVLInit(AVLTree *tree);
int AVLInsert(AVLTree *tree, int value);
int AVLDelete(AVLTree *tree, int value);
int AVLSearch(const AVLTree *tree, int value);
void AVLClear(AVLTree *tree);

int AVLInorder(const AVLTree *tree, int output[], int capacity);
int AVLPreorder(const AVLTree *tree, int output[], int capacity);
int AVLPostorder(const AVLTree *tree, int output[], int capacity);

#endif