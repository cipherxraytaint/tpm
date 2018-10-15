/*
 * stack.h
 *
 *  Created on: Oct 14, 2018
 *      Author: mchen
 */

#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>  // bool
#include <stddef.h>   // size_t

typedef struct StackElet_
{
  void *elet;
  struct StackElet_ *next;
} StackElet;

typedef struct Stack_
{
  size_t size;
  StackElet *top;
} Stack;

Stack   *stackNew(void);
void    stackDel(Stack *stack);

size_t      stackSize(Stack *stack);
bool        stackEmpty(Stack *stack);

void        *stackPeek(Stack *stack);
StackElet   *stackTop(Stack *stack);
StackElet   *stackNextElet(StackElet *stackElet);
void        *stackGetElet(StackElet *stackElet);

void    stackPush(Stack *stack, void *elet);
void    *stackPop(Stack *stack);

#endif /* STACK_H_ */
