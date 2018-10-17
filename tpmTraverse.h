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
