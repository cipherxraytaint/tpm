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

/* dfs clear transition visit flags */
static void
dfs_tpmClearTrans(TPMNode2 *srcNode);

/*
 * operation function
 *   determines which operations to perform.
 */
static void
dfsTrans_operation(TPMNode2 *srcNode, Stack *stack, void *operationCtxt);

/* update buffer hit count array */
static void
dfsNode_updateBufHitCountAry(Stack *stack, void *operationCtxt);

static void
dfsTrans_updateBufHitCountAry(TPMNode2 *srcNode, Stack *stack, void *operationCtxt);

/* write propgate info to 2 level hash files */
static void
dfsTrans_write2LvlHashFile(TPMNode2 *srcNode, Stack *stack, void *w2LvlHashFileCtxt);

/* ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
 * public functions
 * ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
 */
OperationCtxt *
createOperationCtxt(enum OperateType ot, void *ctxt)
{
  OperationCtxt *octx = calloc(1, sizeof(OperationCtxt) );
  assert(octx);

  octx->ot = ot;
  octx->ctxt = ctxt;

  return octx;
}

void
delOperationCtxt(OperationCtxt *operationCtxt)
{
  if(operationCtxt != NULL)
    free(operationCtxt);
}

void
clearTPMVisitFlag(TPMNode2 *srcNode)
{
  dfs_tpmClearTrans(srcNode);
}


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
  Stack *memTransStack = stackNew(); // node stack for mem

  Transition *first = srcNode->firstChild;
  while(first != NULL) {
    stackPush(transStack, first);
    first = first->next;
  }

  // stackPush(memTransStack, srcNode);

  while(!stackEmpty(transStack) ) {
    Transition *topTrans = (Transition *)stackPeek(transStack);
    TPMNode *topNode = topTrans->child;

    if(dfs_isVisitTrans(srcNode, topTrans->hasVisit) ) {
      if(isTPMMemNode(topNode) ) {
        assert(topTrans == (Transition *)stackPeek(memTransStack) );
        stackPop(memTransStack);
      }

      stackPop(transStack);
    }
    else { // new transition
      if(isTPMMemNode(topNode) ) {
        stackPush(memTransStack, topTrans );
        // Transition *trans = stackPeek(memTransStack);
        // printMemNodeLit( &(trans->child->tpmnode2) );
        // dfsTrans_updateBufHitCountAry(srcNode, memTransStack, operationCtxt);
        dfsTrans_operation(srcNode, memTransStack, operationCtxt);
      }

      topTrans->hasVisit = (u32)srcNode; // use the src node ptr val as
      // unique idx to mark if the trans had been visited from the source node

      if(dfs_isLeafNode(topNode) ) {
        if(isTPMMemNode(topNode) ) {
          assert(topTrans == (Transition *)stackPeek(memTransStack) );
          stackPop(memTransStack);
        }
        stackPop(transStack);
      }
      else {
        dfs_traverseChildrenTrans(srcNode, topNode, transStack);
      }
    }
  }

  stackDel(transStack);
  stackDel(memTransStack);
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

static void
dfs_tpmClearTrans(TPMNode2 *srcNode)
{
  Stack *transStack = stackNew(); // transition stack for tpm

  Transition *first = srcNode->firstChild;
  while(first != NULL) {
    stackPush(transStack, first);
    first = first->next;
  }

  while(!stackEmpty(transStack) ) {
    Transition *topTrans = (Transition *)stackPop(transStack); // pop directly
    topTrans->hasVisit = 0;

    TPMNode *topNode = topTrans->child;
    Transition *firstChild = topNode->tpmnode1.firstChild;
    while(firstChild != NULL) {
      if(firstChild->hasVisit != 0 ) {
        stackPush(transStack, firstChild);
      }
      firstChild = firstChild->next;
    }
  }
  stackDel(transStack);
}

/* determine perform which operations */
static void
dfsTrans_operation(TPMNode2 *srcNode, Stack *stack, void *operationCtxt)
{
  OperationCtxt *oCtxt = (OperationCtxt *)operationCtxt;
  if(oCtxt->ot == UPDATE_BUF_HIT_CNT_ARY)
    dfsTrans_updateBufHitCountAry(srcNode, stack, oCtxt->ctxt);
  else if(oCtxt->ot == WRITE_2LVL_HASH)
    dfsTrans_write2LvlHashFile(srcNode, stack, oCtxt->ctxt);
  else{
    fprintf(stderr, "unknown operation types\n");
    abort();
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
  if(stackEmpty(stack))
    return;

  BufHitCountAryCtxt *bufHitCountAryCtxt = (BufHitCountAryCtxt *)operationCtxt;

  Transition *dstTrans = (Transition *)stackPeek(stack); // dst trans is last elet in stack
  TPMNode2 *dstNode = &(dstTrans->child->tpmnode2);

  if(stackSize(stack) <= 1)
    goto updateSrcNode;

  StackElet *topElet = stackTop(stack);
  StackElet *srcElet = stackNextElet(topElet); // src starts from last second
  // elet in stack

  while(srcElet != NULL) {
    Transition *intermediateSrcTrans = (Transition *)stackGetElet(srcElet);
    if(!dfs_isVisitTrans(srcNode, intermediateSrcTrans->hasVisit) ) {

      TPMNode2 * intermediateSrcNode = &(intermediateSrcTrans->child->tpmnode2);
      if(!areSameBuffer(intermediateSrcNode, dstNode) ) {
//        printf("----- \nupdate hit count buffer array:\n");
//        printf("src:\t");
//        printMemNodeLit(srcNode);
//        printf("dst:\t");
//        printMemNodeLit(dstNode);
        // Temporary
        if(intermediateSrcNode->bufid > 0 && dstNode->bufid > 0) {
          u32 srcBufIdx = intermediateSrcNode->bufid - 1;
          u32 dstBufIdx = dstNode->bufid - 1;
          updateBufHitCountAry(bufHitCountAryCtxt->bufHitCountAry, bufHitCountAryCtxt->numBuf,
              srcBufIdx, dstBufIdx, dstNode->bytesz);
        }
      }
    }
    srcElet = stackNextElet(srcElet);
  }

updateSrcNode:
  //intermediate source node doesn't include the source node to the dst node,
  // adds it here
  if(!areSameBuffer(srcNode, dstNode) ) {
    if(srcNode->bufid > 0 && dstNode->bufid > 0) {
      u32 srcBufIdx = srcNode->bufid - 1;
      u32 dstBufIdx = dstNode->bufid - 1;
      updateBufHitCountAry(bufHitCountAryCtxt->bufHitCountAry, bufHitCountAryCtxt->numBuf,
          srcBufIdx, dstBufIdx, dstNode->bytesz);
    }
  }
}

static void
dfsTrans_write2LvlHashFile(TPMNode2 *srcNode, Stack *stack, void *w2LvlHashFileCtxt)
{
  printf("write proopagte info to 2 lvl hash files\n");
}
