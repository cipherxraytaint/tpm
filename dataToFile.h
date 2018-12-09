/*
 * dataToFile.h
 *  mchen
 */

#ifndef DATATOFILE_H_
#define DATATOFILE_H_

#include <stdio.h>  // FILE
#include "bufHitCountArray.h"
#include "tpm.h"
#include "uthash.h"

#define MAX_FILENAME_LEN    128

/*
 * Two level hash table to file ptr
 *  1) src buf ID as 1st level hash
 *  2) dst buf ID as 2nd level hash
 *  <src buf ID, dst buf ID>: File *fl ptr
 */
typedef struct BufPair2FileHashItem_
{
  u32 bufID; // Used as key to hash: src buf ID in 1st level; dst buf ID in 2nd level
  struct BufPair2FileHashItem_ *subHash; // next level hash
  FILE *fl; // the file ptr the <src buf ID, dst buf ID> points to
  UT_hash_handle hh_bufPair2FileItem;  // makes this structure hashable
} BufPair2FileHashItem;

/*
 * Data to file context:
 *  contains context info to write <src,dst> propagation pair
 *  into files.
 */
typedef struct Data2FileCtxt_{
  BufHitCountAryCtxt *bufHitCntAryCtxt;
  BufPair2FileHashItem *bufPair2FileHashHead;
  TPMBufContext *tpmBufCtxt;
} Data2FileCtxt;

/*
 * Buf head information
 */
typedef struct BufHeadInfo_ {
  u32 srcBufBegin;
  u32 srcBufEnd;
  u32 dstBufBegin;
  u32 dstBufEnd;
} BufHeadInfo;

/*
 * <src version node, dst version node> content write to file
 */
typedef struct PropagatePair_ {
//  u32 srcAddr;
//  u8  srcSz;
//  u32 srcVer;
//  u32 srcVal;
//  u32 dstAddr;
//  u8  dstSz;
//  u32 dstVer;
//  u32 dstVal;
  TPMNode2 *src_tr; // src node tpm ptr
  TPMNode2 *dst_ptr;// dst node tpm ptr

} PropagatePair ;


/* Function prototype */

Data2FileCtxt *
newData2FileCtxt(
    BufHitCountAryCtxt *bufHitCntAryCtxt,
    TPMBufContext *tpmBufCtxt);

void
delData2FileCtxt(Data2FileCtxt *data2FlCtxt);

/* ----- ----- ----- ----- ----- ----- ----- -----
 * Buf pair to file hash
 * ----- ----- ----- ----- ----- ----- ----- ----- */

BufPair2FileHashItem *
newBufPair2FileHashItem(u32 bufID, BufPair2FileHashItem *subHash, FILE *fl);

void
delBufPair2FileHashItem(BufPair2FileHashItem *head, BufPair2FileHashItem *bufPair2FileHashItem);

/*
 * Del all buf pair to file items.
 */
void
delBufPair2FileHash(BufPair2FileHashItem *head);

/*
 * Find the BufPair2FileHashItem if it's in hash table
 */
BufPair2FileHashItem *
findBufPair2FileItem(BufPair2FileHashItem *head, u32 bufID);

/* ----- ----- ----- ----- ----- ----- ----- -----
 * File related
 * ----- ----- ----- ----- ----- ----- ----- ----- */

FILE *newFile(u32 srcBufID, u32 dstBufID);

void closeBufPairFile(BufPair2FileHashItem *head);

BufHeadInfo *newBufHeadInfo(
    u32 srcBufBegin,
    u32 srcBufEnd,
    u32 dstBufBegin,
    u32 dstBufEnd);

void
delBufHeadInfo(BufHeadInfo *bufHeadInfo);

PropagatePair *
newPropagatePair(
//    u32 srcAddr,
//    u32 srcVal,
//    u32 dstAddr,
//    u32 dstVal,
    TPMNode2 *src_ptr,
    TPMNode2 *dst_ptr);

void
delPropagatePair(PropagatePair **pp);

void
printPropagatePair(PropagatePair *pp);

#endif /* DATATOFILE_H_ */
