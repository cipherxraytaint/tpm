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

/* dfs traverse node */
static void
dfs_tpmTraverseNode(TPMNode2 *srcNode, void *operationCtxt);

static bool
dfs_isVisitNode(TPMNode2 *srcNode, u32 visitNodeIdx);

static bool
dfs_isLeafNode(TPMNode *node);

static void
dfs_traverseChildrenNode(TPMNode2 *srcNode, TPMNode *farther, Stack *stack);

/* dfs traverse transition */
static void
dfs_tpmTraverseTrans(TPMNode2 *srcNode, void *operationCtxt);

static bool
dfs_isVisitTrans(TPMNode2 *srcNode, u32 visitTransIdx);

static void
dfs_traverseChildrenTrans(TPMNode2 *srcNode, TPMNode *farther, Stack *stack);


/* update buffer hit count array */
static void
dfsNode_updateBufHitCountAry(Stack *stack, void *operationCtxt);

static void
dfsTrans_updateBufHitCountAry(TPMNode2 *srcNode, Stack *stack, void *operationCtxt);

void
tpmTraverse(
    TPMNode2 *srcNode,
    void *operationCtxt)
{
  // dfs_tpmTraverseNode(srcNode, operationCtxt);
  dfs_tpmTraverseTrans(srcNode, operationCtxt);
}

/* static functions */

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 * dfs traverse node
 * ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

static void
dfs_tpmTraverseNode(
    TPMNode2 *srcNode,
    void *operationCtxt)
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
        dfsNode_updateBufHitCountAry(memStack, operationCtxt);
      }

      if(dfs_isLeafNode(node) ) {
        if(isTPMMemNode(node) ) {
          assert((TPMNode2 *)stackPeek(nodeStack) == (TPMNode2 *)stackPeek(memStack) );
          stackPop(memStack);
        }
        stackPop(nodeStack);
      }
      else {
        dfs_traverseChildrenNode(srcNode, node, nodeStack);
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
dfs_traverseChildrenNode(TPMNode2 *srcNode, TPMNode *farther, Stack *stack)
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

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 * dfs traverse transitions
 * ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
static void
dfs_tpmTraverseTrans(TPMNode2 *srcNode, void *operationCtxt)
{
  Stack *transStack = stackNew(); // transition stack for tpm
  Stack *memStack = stackNew();   // node stack for mem

  Transition *first = srcNode->firstChild;
  while(first != NULL) {
    stackPush(transStack, first);
    first = first->next;
  }

  stackPush(memStack, srcNode);

  while(!stackEmpty(transStack) ) {
    Transition *topTrans = (Transition *)stackPeek(transStack);
    TPMNode *topNode = topTrans->child;

    if(dfs_isVisitTrans(srcNode, topTrans->hasVisit) ) {
      if(isTPMMemNode(topNode) ) {
        assert(&(topNode->tpmnode2) == (TPMNode2 *)stackPeek(memStack) );
        stackPop(memStack);
      }

      stackPop(transStack);
    }
    else { // new transition
      topTrans->hasVisit = (u32)srcNode; // use the src node ptr val as
      // unique idx to mark if the trans had been visited from the source node

      if(isTPMMemNode(topNode) ) {
        stackPush(memStack, &(topNode->tpmnode2) );
        // printMemNodeLit((TPMNode2 *)stackPeek(memStack) );
        dfsTrans_updateBufHitCountAry(srcNode, memStack, operationCtxt);
      }

      if(dfs_isLeafNode(topNode) ) {
        if(isTPMMemNode(topNode) ) {
          assert(&(topNode->tpmnode2) == (TPMNode2 *)stackPeek(memStack) );
          stackPop(memStack);
        }
        stackPop(transStack);
      }
      else {
        dfs_traverseChildrenTrans(srcNode, topNode, transStack);
      }
    }
  }

  stackDel(transStack);
  stackDel(memStack);
}

static bool
dfs_isVisitTrans(TPMNode2 *srcNode, u32 visitTransIdx)
{
  if((u32)(srcNode) == visitTransIdx)
    return true;
  else
    return false;
}

static void
dfs_traverseChildrenTrans(TPMNode2 *srcNode, TPMNode *farther, Stack *stack)
{
  Transition *firstChild = farther->tpmnode1.firstChild;
  while(firstChild != NULL) {
    if(!dfs_isVisitTrans(srcNode, firstChild->hasVisit) ) {
      stackPush(stack, firstChild);
    }
    firstChild = firstChild->next;
  }
}

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 * update buffer hit count array related
 * ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */
static void
dfsNode_updateBufHitCountAry(
    Stack *stack,
    void *operationCtxt)
{
  if(stackSize(stack) < 2)
    return;

  BufHitCountAryCtxt *bufHitCountAryCtxt = (BufHitCountAryCtxt *)operationCtxt;

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
        updateBufHitCountAry(bufHitCountAryCtxt->bufHitCountAry, bufHitCountAryCtxt->numBuf,
            srcBufIdx, dstBufIdx, dstNode->bytesz);
      }
    }

    srcElet = stackNextElet(srcElet);
  }
}

static void
dfsTrans_updateBufHitCountAry(TPMNode2 *srcNode, Stack *stack, void *operationCtxt)
{
  if(stackSize(stack) < 2)
    return;

  BufHitCountAryCtxt *bufHitCountAryCtxt = (BufHitCountAryCtxt *)operationCtxt;

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
        updateBufHitCountAry(bufHitCountAryCtxt->bufHitCountAry, bufHitCountAryCtxt->numBuf,
            srcBufIdx, dstBufIdx, dstNode->bytesz);
      }
    }

    srcElet = stackNextElet(srcElet);
  }

}
