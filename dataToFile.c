/*
 * dataToFile.c
 *  mchen
 */

#include <assert.h>
#include <stdlib.h> // calloc
#include <stdio.h>
#include "dataToFile.h"

static void
newFileName(char *fileName, u32 srcBufID, u32 dstBufID);

/* ----- ----- ----- ----- ----- ----- ----- -----
 * Public function
 * ----- ----- ----- ----- ----- ----- ----- ----- */

Data2FileCtxt *
newData2FileCtxt(
    BufHitCountAryCtxt *bufHitCntAryCtxt,
    TPMBufContext *tpmBufCtxt)
{
  Data2FileCtxt *data2FlCtxt = calloc(1, sizeof(Data2FileCtxt) );
  assert(data2FlCtxt);

  data2FlCtxt->bufHitCntAryCtxt = bufHitCntAryCtxt;
  data2FlCtxt->bufPair2FileHashHead = NULL;
  data2FlCtxt->tpmBufCtxt = tpmBufCtxt;

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

/* ----- ----- ----- ----- ----- ----- ----- -----
 * Buf pair to file hash
 * ----- ----- ----- ----- ----- ----- ----- ----- */

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

FILE *newFile(u32 srcBufID, u32 dstBufID)
{
  char fn[MAX_FILENAME_LEN] = {0};
  newFileName(fn, srcBufID, dstBufID);
  printf("file name:%s\n", fn);

  FILE *fl = fopen(fn,"w+b");
  assert(fl);

  return fl;
}

void closeBufPairFile(BufPair2FileHashItem *head)
{
  BufPair2FileHashItem *it, *tmp;

  HASH_ITER(hh_bufPair2FileItem, head, it, tmp) {
    BufPair2FileHashItem *sub_it, *subTmp;

    HASH_ITER(hh_bufPair2FileItem, it->subHash, sub_it, subTmp) {
      if(sub_it->fl)
        fclose(sub_it->fl);
    }
  }
  printf("close buf pair 2 files\n");
}

void readBufPairFile(FILE *fl)
{
  rewind(fl);
  BufHeadInfo *bh = newBufHeadInfo(NULL,NULL);
  PropagatePair *pp = newPropagatePair(NULL,NULL);

  // Read buf head info
  if(fread(bh,sizeof(BufHeadInfo),1,fl) < 0) {
    fprintf(stderr, "error read buf head info from file\n");
    goto EXIT;
  }
  printBufHeadInfo(bh);

  // Read <src node, dst node> propagate
  int nread = 0;
  while((nread = fread(pp, sizeof(PropagatePair), 1, fl) ) > 0) {
    printPropagatePair(pp);
  }

EXIT:
  delBufHeadInfo(bh);
  delPropagatePair(&pp);
}

BufHeadInfo *newBufHeadInfo(
//    u32 srcBufBegin,
//    u32 srcBufEnd,
//    u32 dstBufBegin,
//    u32 dstBufEnd,
    TPMBufHashTable *srcBuf,
    TPMBufHashTable *dstBuf)
{
  BufHeadInfo *bh = calloc(1, sizeof(BufHeadInfo) );
  assert(bh);

//  bh->srcBufBegin = srcBufBegin;
//  bh->srcBufEnd = srcBufEnd;
//  bh->dstBufBegin = dstBufBegin;
//  bh->dstBufEnd = dstBufEnd;

  bh->srcBuf = srcBuf;
  bh->dstBuf = dstBuf;
  return bh;
}

void
delBufHeadInfo(BufHeadInfo *bufHeadInfo)
{
  if(bufHeadInfo)
    free(bufHeadInfo);
}

PropagatePair *
newPropagatePair(
//    u32 srcAddr,
//    u32 srcVal,
//    u32 dstAddr,
//    u32 dstVal,
    TPMNode2 *src_ptr,
    TPMNode2 *dst_ptr)
{
  PropagatePair *pp = calloc(1, sizeof(PropagatePair));
  assert(pp != NULL);

//  pp->srcAddr   = srcAddr;
//  pp->srcVal    = srcVal;
//  pp->dstAddr   = dstAddr;
//  pp->dstVal    = dstVal;
  pp->src_tr = src_ptr;
  pp->dst_ptr = dst_ptr;

  return pp;
}

void delPropagatePair(PropagatePair **pp)
{
  assert(pp != NULL);
  if(*pp != NULL) {
    free(*pp);
    *pp = NULL;
  }
}

void
printBufHeadInfo(BufHeadInfo *bh)
{
  if(bh) {
//    printf("Buf head: srcBufB:0x%x srcBufE:0x%x - dstBufB:0x%x dstBufE:0x%x\n",
//            bh->srcBufBegin,bh->srcBufEnd,bh->dstBufBegin,bh->dstBufEnd);
    printf("Buf head: srcBuf:%p - dstBuf:%p\n",
            bh->srcBuf,bh->dstBuf);
  }
}

void
printPropagatePair(PropagatePair *pp)
{
  if(pp) {
//    printf("propagatepair: srcAddr:%u - srcVal:%u - dstAddr:%u - dstVal:%u\n",
//           pp->srcAddr, pp->srcVal, pp->dstAddr, pp->dstVal);
    printf("propagatepair:srcptr:%p - dstptr:%p\n",pp->src_tr, pp->dst_ptr);
  }
}

/* ----- ----- ----- ----- ----- ----- ----- -----
 * static function
 * ----- ----- ----- ----- ----- ----- ----- ----- */

static void
newFileName(char *fileName, u32 srcBufID, u32 dstBufID)
{
  sprintf(fileName, "%d_%d", srcBufID, dstBufID);
}

