/*
 * bufHitCountArray.h
 *  Author: mchen
 *
 *  buffer hit count array is a 2D array, each elet records hit counts (propagations) from
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

#ifndef BUFHITCOUNTARRAY_H_
#define BUFHITCOUNTARRAY_H_

#include "type.h"

#define HIT_COUNT_BYTE unsigned char// 1 byte
#define MAX_HIT_COUNT 255 // max hit count of 1 byte

typedef HIT_COUNT_BYTE *BufHitCountAry_T;

/*
 * Allocates a 8*numBuf*numBuf bytes array.
 * Returns the BufHitCountAry_T type (a pointer)
 */
extern BufHitCountAry_T newBufHitCountAry(u32 numBuf);

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
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf,
    u32 srcBufID,
    u32 dstBufID,
    u8 hitCountByte);

/*
 * Del the buffer hit count array, passes a BufHitCountAry_T **
 */
extern void delBufHitCountAry(BufHitCountAry_T *bufHitCountAry);

extern void statBufHitCountArray(
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf,
    u32 byteThreashold);


extern void printBufHitCountAry(
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf);

#endif /* BUFHITCOUNTARRAY_H_ */
