/*
 * bufHitCountArray.c
 *  Author: mchen
 */

#include <assert.h>
#include <stdio.h>
#include "bufHitCountArray.h"

BufHitCountAry_T newBufHitCountAry(u32 numBuf)
{
  BufHitCountAry_T bufHitCountAry = NULL;

  bufHitCountAry = calloc(numBuf*numBuf, sizeof(HIT_COUNT_BYTE) );
  assert(bufHitCountAry);

  return bufHitCountAry;
}

void delBufHitCountAry(BufHitCountAry_T *bufHitCountAry)
{
  assert(bufHitCountAry);
  free(*bufHitCountAry);
  *bufHitCountAry = NULL;
  printf("del buffer hit count array\n");
}
