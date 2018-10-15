/*
 * tpmTraverse.c
 *
 *  Created on: Oct 15, 2018
 *      Author: mchen
 */

#include <assert.h>
#include <stdbool.h>
#include "tpmTraverse.h"
#include "stack.h"
#include "tpm.h"

/* dfs */
static void dfs_tpmTraverse(TPMNode2 *srcNode, BufHitCountAry_T bufHitCountAry, u32 numBuf);
static bool dfs_isVisitNode(TPMNode2 *srcNode, u32 visitNodeIdx);
static bool dfs_isLeafNode(TPMNode *node);
static void dfs_traverseChildren(TPMNode2 *srcNode, TPMNode *farther, Stack *stack);

/* update buffer hit count array */
static void dfs_updateBufHitCountAry(Stack *stack, BufHitCountAry_T bufHitCountAry, u32 numBuf);

void
tpmTraverse(
    TPMNode2 *srcNode,
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf)
{
  dfs_tpmTraverse(srcNode, bufHitCountAry, numBuf);
}

/* static functions */
static void
dfs_tpmTraverse(TPMNode2 *srcNode, BufHitCountAry_T bufHitCountAry, u32 numBuf)
{
  Stack *nodeStack = stackNew();    // node stack for both mem and non-mem type
  Stack *memStack = stackNew();     // node stack for mem

  stackPush(nodeStack, srcNode);
  // stackPush(memStack, srcNode);

  while(!stackEmpty(nodeStack) ) {
    TPMNode *node = (TPMNode *)(stackPeek(nodeStack) );

    if(dfs_isVisitNode(srcNode, node->tpmnode1.visitNodeIdx) ) { // visited node
      if(isTPMMemNode(node) ) {
        assert((TPMNode2 *)stackPeek(nodeStack) == (TPMNode2 *)stackPeek(memStack) );
        stackPop(memStack);
      }

      stackPop(nodeStack);
    }
    else{ // unvisited node
      node->tpmnode1.visitNodeIdx = (u32)srcNode; // use the src node ptr val as
      // unique idx to mark if the node had been visited from the source node
      // printf("visitNodeIdx:0x%x\n", node->tpmnode1.visitNodeIdx);

      if(isTPMMemNode(node) ) {
        stackPush(memStack, node);
        // printMemNodeLit((TPMNode2 *)stackPeek(memStack) );
        dfs_updateBufHitCountAry(memStack, bufHitCountAry, numBuf);
      }

      if(dfs_isLeafNode(node) ) {
        if(isTPMMemNode(node) ) {
          assert((TPMNode2 *)stackPeek(nodeStack) == (TPMNode2 *)stackPeek(memStack) );
          stackPop(memStack);
        }
        stackPop(nodeStack);
      }
      else {
        dfs_traverseChildren(srcNode, node, nodeStack);
      }
    }
  }

  stackDel(nodeStack);
  stackDel(memStack);
}

static bool
dfs_isVisitNode(
    TPMNode2 *srcNode,
    u32 visitNodeIdx)
{
  if((u32)(srcNode) == visitNodeIdx)
    return true;
  else
    return false;
}

static bool
dfs_isLeafNode(TPMNode *node)
{
  if(node->tpmnode1.firstChild == NULL)
    return true;
  else
    return false;
}

static void
dfs_traverseChildren(TPMNode2 *srcNode, TPMNode *farther, Stack *stack)
{
  Transition *firstChild = farther->tpmnode1.firstChild;
  while(firstChild != NULL) {
    TPMNode *child = firstChild->child;
    // Only traverse unvisited node
    if(!dfs_isVisitNode(srcNode, child->tpmnode1.visitNodeIdx) ) {
      stackPush(stack, child);
    }
    firstChild = firstChild->next;
  }
}

/* update buffer hit count array related */
static void
dfs_updateBufHitCountAry(
    Stack *stack,
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf)
{
  if(stackSize(stack) < 2)
    return;

  StackElet *topElet = stackTop(stack);
  StackElet *srcElet = stackNextElet(topElet); // src starts from last second
  // elet in stack

  TPMNode2 *dstNode = (TPMNode2 *)stackPeek(stack); // dst node is last elet in stack
  while(srcElet != NULL) {
    TPMNode2 * srcNode = (TPMNode2 *)stackGetElet(srcElet);
    if(!areSameBuffer(srcNode, dstNode) ) {
//      printf("----- \nupdate hit count buffer array:\n");
//      printf("src:\t");
//      printMemNodeLit(srcNode);
//      printf("dst:\t");
//      printMemNodeLit(dstNode);
      // Temporary
      if(srcNode->bufid > 0 && dstNode->bufid > 0) {
        u32 srcBufIdx = srcNode->bufid - 1;
        u32 dstBufIdx = dstNode->bufid - 1;
        updateBufHitCountAry(bufHitCountAry, numBuf, srcBufIdx, dstBufIdx, dstNode->bytesz);
      }
    }

    srcElet = stackNextElet(srcElet);
  }
}
