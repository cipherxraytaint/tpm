/*
 * stack.c
 *
 *  Created on: Oct 14, 2018
 *      Author: mchen
 */

#include <assert.h>
#include <stdlib.h> // calloc
#include "stack.h"

Stack *
stackNew(void)
{
  Stack *stack = calloc(1, sizeof(Stack) );
  assert(stack);
  stack->size = 0;
  stack->top  = NULL;
  return stack;
}

void
stackDel(Stack *stack)
{
  while(stack->size > 0)
    stackPop(stack);
  free(stack);
}

size_t
stackSize(Stack *stack)
{
  return stack->size;
}

bool
stackEmpty(Stack *stack)
{
  return (stack->size == 0) ? true : false;
}

void *
stackPeek(Stack *stack)
{
  if(!stackEmpty(stack) ) {
    return stack->top->elet;
  }
  else
    return NULL;
}

StackElet *
stackTop(Stack *stack)
{
  if(!stackEmpty(stack) )
    return stack->top;
  else
    return NULL;
}

StackElet *
stackNextElet(StackElet *stackElet)
{
  if(stackElet != NULL)
    return stackElet->next;
  else
    return NULL;
}

void *
stackGetElet(StackElet *stackElet)
{
  if(stackElet != NULL)
    return stackElet->elet;
  else
    return NULL;
}

void
stackPush(Stack *stack, void *elet)
{
  StackElet *stackElet = calloc(1, sizeof(StackElet) );
  assert(stackElet);

  stackElet->elet = elet;
  stackElet->next = stack->top;

  stack->top  = stackElet;
  stack->size += 1;
}

void *
stackPop(Stack *stack)
{
  StackElet *toPop;
  void *elet = NULL;

  if(!stackEmpty(stack) ) {
    toPop = stack->top;
    stack->top = toPop->next;
    stack->size -= 1;

    elet = toPop->elet;
    free(toPop);
  }
  return elet;
}
