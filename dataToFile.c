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
  return data2FlCtxt;
}

void
delData2FileCtxt(Data2FileCtxt *data2FlCtxt)
{
  if(data2FlCtxt)
    free(data2FlCtxt);
  printf("del data to file context\n");
}
