/*
 * dataToFile.h
 *  mchen
 */

#ifndef DATATOFILE_H_
#define DATATOFILE_H_

#include <stdio.h>  // FILE
#include "bufHitCountArray.h"
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
} Data2FileCtxt;

Data2FileCtxt *
newData2FileCtxt(BufHitCountAryCtxt *bufHitCntAryCtxt);

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

#endif /* DATATOFILE_H_ */
