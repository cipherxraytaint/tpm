/*
 * tpmTraverse.h
 *
 *  Created on: Oct 15, 2018
 *      Author: mchen
 *  Provide APIs to traverse tpm, extend propagate.h/c files.
 */

#ifndef TPM_TRAVERSE_H_
#define TPM_TRAVERSE_H_

#include "tpmnode.h"

/*
 * Traverse the whole tpm based on a memory type source node.
 */
void
tpmTraverse(TPMNode2 *srcNode);

#endif /* TPM_TRAVERSE_H_ */
