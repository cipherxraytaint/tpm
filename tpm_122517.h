/*
 * tpm.h
 * 
 * header file of Taint Propagation Map
 * 
 * created on 12/8/2017
 */

#include <stdio.h>
#include "uthash.h"

#ifndef TPM_H
#define TPM_H

#define u32	unsigned int

/* need to add all the XTaint record flag definition here */


/* TPM related constants */

/* the following 2 constants need to be adjuested based on statistics of the XTaint log */
#define mem2NodeHashSize	90000
#define seqNo2NodeHashSize	20000000

#define TPM_Type_Register	0x00000001
#define TPM_Type_Temprary	0x00000002
#define TPM_Type_Memory		0x00000004

struct Transition;

struct TPMNode1		// for temp, register addresses
{
    u32	type;		// indicating the type of the node
    u32	addr;		// mem addr, id of temp or register
    u32	lastUpdateTS;	// the TS (seq#) of last update of the node
    struct Transition *firstChild;  // points to structure that points to the first child
};

struct TPMNode2		// for memory address
{
    u32	type;		// indicating the type of the node
    u32	addr;		// mem addr, id of temp or register
    u32	lastUpdateTS;	// the TS (seq#) of last update of the node
    struct Transition *firstChild;  // points to structure that points to the first child
/* the following fields are only for TPMNode for memory */
    struct TPMNode2 *leftNBR;	// point to node of adjacent, smaller memory address 
    struct TPMNode2 *rightNBR;	// point to node of adjacent, bigger memory address 
    struct TPMNode2 *nextVersion;// point to node of the same addr buf of different version or age. Forms circular link
    u32 version;	// the version of current node, monotonically increasing from 0.
    u32	hitcnt;		/* as source, the number of TMPNode2 in destination buffer this node propagates to; or
			   as detination, the number of TMPNode2 in source buffer that propagates to this node	*/
};

union TPMNode
{
    struct TPMNode1 tpmnode1;
    struct TPMNode2 tpmnode2;
};

struct Transition
{
    u32 seqNo;		// sequence number of corresponding XTaint record
    union TPMNode *child;
    struct Transition *next;
};

struct taintedBuf
{
    struct TPMNode2 *bufstart;	// point to the TMPNode2 of the start addr of some tainted buffer in TPM;
    struct taintedBuf *next;	// point to the taintedBuf structure of the next tainted buffer; null if no more
};

struct TPMContext
{
    u32 nodeNum;	// total number of TPM node
    u32 memAddrNum;	// number of different memory addresses encountered
    u32 tempVarNum;	// number of different temporary variables encounted
    struct TPMNode2 *mem2NodeHash[mem2NodeHashSize];	// maps mem addr to TPMNode2 of the latest version of a mem addr
/* the following two fields are intented to be used for freeing the nod ememory. They can be put into file instead of mem */
    union TMPNode *seqNo2SrcNodeHash[seqNo2NodeHashSize];	// maps seq no. to TPMNode of the source of the transision
    union TMPNode *seqNo2DstNodeHash[seqNo2NodeHashSize];	// maps seq no. to TPMNode of the destination of the transision
    u32 minBufferSz;	// minimum buffer size (such as 8) considered for avalanche effect search
    u32 taintedBufNum;	// number of tainted buffers in the TPM.
    struct taintedBuf *taintedbuf;	// point to the tainted buffers in TPM
};

struct addr2NodeItem
{
    u32 addr;			/* 32-bit address: src addr in 1st level hash; dst addr in 2nd level hash */
    struct TMPNode2 *node;	/* used as key to hash: src node in 1st level hash; dst node in 2nd level hash */
    struct addr2NodeItem *subHash;	/* next level hash */
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct AvalancheSearchCtxt
{
    u32 minBufferSz;		// minimum buffer size (such as 8) considered for avalanche effect search
    struct TPMNode2 *srcBuf;	// point to potential source buffer
    struct TPMNode2 *dstBuf;	// point to potential destination buffer
    u32	srcAddrStart;		// starting addr of the potential source buffer
    u32 srcAddrEnd;		// end addr of the potential source buffer. Should be >= srcAddrStart
    u32	dstAddrStart;		// starting addr of the potential destination buffer
    u32 dstAddrEnd;		// end addr of the potential destination buffer. Should be >= dstAddrStart
    u32 srcMinSeqN;		// minimum seq# of the source buffer
    u32 srcMaxSeqN;		// maximum seq# of the source buffer
    u32 dstMinSeqN;		// minimum seq# of the destination buffer
    u32 dstMaxSeqN;		// maximum seq# of the destination buffer
    unsigned char *srcAddrHitCnt;  // array[0, srcAddrEnd-srcAddrStart] to record the aggregated hit account of all versions of each source address
    unsigned char *dstAddrHitCnt;  // array[0, dstAddrEnd-dstAddrStart] to record the aggregated hit account of all versions of each destination address
    struct addr2NodeItem **addr2Node;	// array[0, dstAddrEnd-dstAddrStart] of pointers to struct addr2NodeItem (hash table)
};


/* TPM function prototypes */

u32 
isPropagationOverwriting(u32 flag);

union TPMNode *
createTPMNode(u32 type, u32 addr, u32 TS);

u32 
processOneXTaintRecord(struct TPMContext *tpm, u32 seqNo, u32 size, u32 srcflg, u32 srcaddr, u32 dstflag, u32 dstaddr);

u32 
buildTPM(FILE *taintfp, struct TPMContext *tpm);

struct TPMNode2 *
mem2NodeSearch(struct TPMContext *tpm, u32 memaddr);

union TPMNode *
seqNo2NodeSearch(struct TPMContext *tpm, u32 seqNo);

int
init_AvalancheSearchCtxt(struct AvalancheSearchCtxt *avalsctxt, u32 minBufferSz, struct TPMNode2 *srcBuf, 
			 struct TPMNode2 *dstBuf, u32 srcAddrStart, u32 srcAddrEnd, u32 dstAddrStart, u32 dstAddrEnd);

void
free_AvalancheSearchCtxt(struct AvalancheSearchCtxt *avalsctxt);

struct TPMNode2 * 
memNodeReachBuf(struct AvalancheSearchCtxt *avalsctxt, struct TPMNode2 *srcNode, struct taintedBuf *dstBuf);
/* return:
    NULL: srcNode does not reach any node in the dstBuf
    else: pointer to the node in dstBuf that srcNode reaches
*/

int
memNodePropagationSearch(struct AvalancheSearchCtxt *avalsctxt, struct TPMNode2 *srcNode, struct taintedBuf *dstBuf);





#endif