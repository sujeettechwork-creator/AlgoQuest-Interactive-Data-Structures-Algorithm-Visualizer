#include "stack.h"

void StackInit(Stack *stack)
{
    stack->top = -1;
}

int StackIsEmpty(const Stack *stack)
{
    return stack->top == -1;
}

int StackIsFull(const Stack *stack)
{
    return stack->top == STACK_CAPACITY - 1;
}

int StackPush(Stack *stack, int value)
{
    if (StackIsFull(stack))
        return 0;

    stack->top++;
    stack->items[stack->top] = value;
    return 1;
}

int StackPop(Stack *stack, int *removedValue)
{
    if (StackIsEmpty(stack))
        return 0;

    *removedValue = stack->items[stack->top];
    stack->top--;
    return 1;
}

int StackPeek(const Stack *stack, int *topValue)
{
    if (StackIsEmpty(stack))
        return 0;

    *topValue = stack->items[stack->top];
    return 1;
}

int StackSize(const Stack *stack)
{
    return stack->top + 1;
}
