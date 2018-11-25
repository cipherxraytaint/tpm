/*
 * dataToFile.h
 *  mchen
 */

#ifndef DATATOFILE_H_
#define DATATOFILE_H_

#include "bufHitCountArray.h"

typedef struct Data2FileCtxt_{
  BufHitCountAryCtxt *bufHitCntAryCtxt;
} Data2FileCtxt;

Data2FileCtxt *
newData2FileCtxt(BufHitCountAryCtxt *bufHitCntAryCtxt);

void
delData2FileCtxt(Data2FileCtxt *data2FlCtxt);

#endif /* DATATOFILE_H_ */
