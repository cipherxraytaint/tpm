/*
 * dataToFile.c
 *  mchen
 */

#include <assert.h>
#include <stdlib.h> // calloc
#include <stdio.h>
#include "dataToFile.h"

Data2FileCtxt *
newData2FileCtxt(BufHitCountAryCtxt *bufHitCntAryCtxt)
{
  Data2FileCtxt *data2FlCtxt = calloc(1, sizeof(Data2FileCtxt) );
  assert(data2FlCtxt);

  data2FlCtxt->bufHitCntAryCtxt = bufHitCntAryCtxt;
  data2FlCtxt->bufPair2FileHashHead = NULL;
  return data2FlCtxt;
}

// TODO: del BufPair2FileHashItem
void
delData2FileCtxt(Data2FileCtxt *data2FlCtxt)
{
  if(data2FlCtxt)
    free(data2FlCtxt);
  printf("del data to file context\n");
}

BufPair2FileHashItem *
newBufPair2FileHashItem(u32 bufID, BufPair2FileHashItem *subHash, FILE *fl)
{
  BufPair2FileHashItem *bufPair2FileHashItem = calloc(1, sizeof(BufPair2FileHashItem) );
  assert(bufPair2FileHashItem);

  bufPair2FileHashItem->bufID = bufID;
  bufPair2FileHashItem->subHash = subHash;
  bufPair2FileHashItem->fl = fl;

  return bufPair2FileHashItem;
}

void
delBufPair2FileHashItem(BufPair2FileHashItem *head, BufPair2FileHashItem *bufPair2FileHashItem)
{
  if(head && bufPair2FileHashItem) {
    HASH_DELETE(hh_bufPair2FileItem,head,bufPair2FileHashItem);
    free(bufPair2FileHashItem);
  }
}

void
delBufPair2FileHash(BufPair2FileHashItem *head)
{
  BufPair2FileHashItem *del, *tmp;
  HASH_ITER(hh_bufPair2FileItem, head, del, tmp) {
    // Del sub hash
    BufPair2FileHashItem *subDel, *subTmp;
    HASH_ITER(hh_bufPair2FileItem, del->subHash, subDel, subTmp) {
      delBufPair2FileHashItem(del->subHash, subDel);
    }

    delBufPair2FileHashItem(head, del);
  }
  printf("del buf pair 2 file two level hash table\n");
}

BufPair2FileHashItem *
findBufPair2FileItem(BufPair2FileHashItem *head, u32 bufID)
{
  BufPair2FileHashItem *find = NULL;
  HASH_FIND(hh_bufPair2FileItem, head, &bufID, 4, find);
  return find;
}
