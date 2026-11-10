#ifndef ALGOQUEST_STACK_H
#define ALGOQUEST_STACK_H

#define STACK_CAPACITY 8

typedef struct
{
    int items[STACK_CAPACITY];
    int top;
} Stack;

void StackInit(Stack *stack);
int StackIsEmpty(const Stack *stack);
int StackIsFull(const Stack *stack);
int StackPush(Stack *stack, int value);
int StackPop(Stack *stack, int *removedValue);
int StackPeek(const Stack *stack, int *topValue);
int StackSize(const Stack *stack);

#endif
