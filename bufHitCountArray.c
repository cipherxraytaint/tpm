/*
 * bufHitCountArray.c
 *  Author: mchen
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> // calloc
#include "bufHitCountArray.h"

BufHitCountAry_T newBufHitCountAry(u32 numBuf)
{
  BufHitCountAry_T bufHitCountAry = NULL;

  bufHitCountAry = calloc(numBuf*numBuf, sizeof(HIT_COUNT_BYTE) );
  assert(bufHitCountAry);

  printf("with total buf:%u, create buffer hit count array:%p successful\n",
      numBuf, bufHitCountAry);
  return bufHitCountAry;
}

int updateBufHitCountAry(
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf,
    u32 srcBufID,
    u32 dstBufID,
    u8 hitCountByte)
{
  if(bufHitCountAry == NULL ||
     srcBufID >= numBuf ||
     dstBufID >= numBuf)
    return -1;

  HIT_COUNT_BYTE val = bufHitCountAry[srcBufID*numBuf + dstBufID];
  if(val+hitCountByte <= MAX_HIT_COUNT) // if sum not exceed max
    bufHitCountAry[srcBufID*numBuf + dstBufID] = val + hitCountByte;

  return 0;
}

void delBufHitCountAry(BufHitCountAry_T *bufHitCountAry)
{
  assert(bufHitCountAry);
  free(*bufHitCountAry);
  *bufHitCountAry = NULL;
  printf("del buffer hit count array:%p successful\n", *bufHitCountAry);
}

void statBufHitCountArray(
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf,
    u32 byteThreashold)
{
  u32 hitThreash = 0;

  for(size_t r = 0; r < numBuf; r++) {
    for (size_t c = 0; c < numBuf; c++) {
      HIT_COUNT_BYTE val = bufHitCountAry[r*numBuf + c];
      if(val >= byteThreashold)
        hitThreash++;
    }
  }
  printf("----------\nnum of buf pair hitcnt > %u bytes:%u - total buf pair:%u - ratio:%u%%\n",
      byteThreashold, hitThreash, numBuf*numBuf, (hitThreash * 100) / (numBuf*numBuf) );

}

void printBufHitCountAry(
    BufHitCountAry_T bufHitCountAry,
    u32 numBuf)
{
  for(size_t r = 0; r < numBuf; r++) {
    for (size_t c = 0; c < numBuf; c++) {
      HIT_COUNT_BYTE val = bufHitCountAry[r*numBuf + c];
      printf("2D buf hit count array[%zu][%zu]:%u\n", r, c, val);
    }
  }
}
