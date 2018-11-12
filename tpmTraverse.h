/*
 * tpmTraverse.h
 *
 *  Created on: Oct 15, 2018
 *      Author: mchen
 *  Provide APIs to traverse tpm, extend propagate.h/c files.
 */

#ifndef TPM_TRAVERSE_H_
#define TPM_TRAVERSE_H_

#include "bufHitCountArray.h"
#include "tpmnode.h"

/*
 * Operation types
 */
enum OperateType {
  UPDATE_BUF_HIT_CNT_ARY,   // update 2D buffer hit count array
  WRITE_2LVL_HASH,          // write propagation (2 level hash) to files
};

/*
 * Store operation context information. There might be multi-type operations
 * performed in tpm traversing. Use operation context to differenciate which
 * operations to perform.
 */
typedef struct OperationCtxt_
{
  enum OperateType ot;  // operation type
  void *ctxt;           // Specific context
} OperationCtxt;

OperationCtxt *
createOperationCtxt(enum OperateType ot, void *ctxt);

void
delOperationCtxt(OperationCtxt *operationCtxt);

/*
 * Traverse the whole tpm based on a memory type source node and perform
 * needed operations.
 *  @srcNode:
 *   the source memory node starts traversing
 *  @operationCtxt:
 *   the performing operations context during traversing
 */
void tpmTraverse(
    TPMNode2 *srcNode,
    void *operationCtxt);

#endif /* TPM_TRAVERSE_H_ */
