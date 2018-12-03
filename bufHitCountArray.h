/*
 * bufHitCountArray.h
 *  Author: mchen
 *
 *  Buffer hit count array for tpm, extends bufhitcnt.* files.
 *
 *  Buffer hit count array is a 2D array, each elet records hit counts (propagations) from
 *  a src buffer to a dst buffer. For example,
 *   buf 1  2  3...
 *    1     64 32 ...
 *    2
 *    3
 *    ...
 *
 *   the hit counts from buffer 1 to 2: 64, 1 to 3: 32, etc.
 *
 *  Currently the maximum hit count bytes is 255 (1 byte width), if a elet reaches its
 *  maximum, do not update any further.
 */

#ifndef BUF_HITCOUNTARRAY_H_
#define BUF_HITCOUNTARRAY_H_

#include "type.h"

#define HIT_COUNT_BYTE unsigned char // 1 byte
#define MAX_HIT_COUNT 255            // max hit count of 1 byte

typedef HIT_COUNT_BYTE *BufHitCountAry; // unsigned char *

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 * Buffer hit count array functions.
 * ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

/*
 * Allocates a 8*numBuf*numBuf bytes array.
 * Returns the BufHitCountAry_T type (a pointer)
 */
extern BufHitCountAry newBufHitCountAry(u32 numBuf);

/*
 * Updates the buffer hit count array as:
 *  srcBufID --> dstBufID: hitCountByte
 *
 * Currently the maximum hit count bytes is 255 (1 byte), if a elet reaches its
 *  maximum, do not update any further.
 *
 * Returns
 *  0, success
 *  otherwise, fail
 */
extern int updateBufHitCountAry(
    BufHitCountAry bufHitCountAry,
    u32 numBuf,
    u32 srcBufID,
    u32 dstBufID,
    u8 hitCountByte);

/*
 * Del the buffer hit count array, passes a BufHitCountAry_T **
 */
extern void delBufHitCountAry(BufHitCountAry *bufHitCountAry);

extern u32 statBufHitCountArray(
    BufHitCountAry bufHitCountAry,
    u32 numBuf,
    u32 byteThreashold);

extern void clearBufHitCountAry(
    BufHitCountAry bufHitCountAry,
    u32 numBuf);

extern void printBufHitCountAry(
    BufHitCountAry bufHitCountAry,
    u32 numBuf);

/*
 * Buffer hit count array context
 *  needed to perform related operations. For example, if a tpm traverser wants
 *  to update the corresponding buffer hit count array, it needs these context
 *  information
 */
typedef struct BufHitCountAryCtxt_ {
  BufHitCountAry bufHitCountAry;    // the buf hit count array
  u32 numBuf;   // number of buffers of the buf hit count array
} BufHitCountAryCtxt;

/* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** *****
 * Buffer hit count array context functions.
 * ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

BufHitCountAryCtxt *newBufHitCountAryCtxt(BufHitCountAry bufHitCountAry, u32 numBuf);
void delBufHitCountAryCtxt(BufHitCountAryCtxt **bufHitCountAryCtxt);

#endif /* BUF_HITCOUNTARRAY_H_ */
