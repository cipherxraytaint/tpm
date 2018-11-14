#include <stdlib.h>

#include "avalanche.h"
#include "bufhitcnt.h"
#include "bufHitCountArray.h" // hit count buffer array for tpm
#include "env.h"
#include "hitmap.h"
#include "hitmapavalanche.h"
#include "misc.h"
#include "stat.h"
#include "tpm.h"
#include "tpmTraverse.h"

void usage()
{
  printf("usage:\ttpm <log file path>\n");
}

int main(int argc, char const *argv[])
{
  FILE *log;
  struct TPMContext* tpm;
  HitMapContext *hitMap;
  int numOfTPMNode;
  TPMBufContext *tpmBufCtxt;
  u8 *bufHitCntArray = NULL;

  if(argc <= 1){
    usage();
    exit(1);
  }

  if((log = fopen(argv[1], "r") ) != NULL) {
    printf("open log: %s\n", argv[1]);

    if((tpm = calloc(sizeof(struct TPMContext), 1) ) != NULL) {
      printf("alloc TPMContext: %zu MB\n", sizeof(struct TPMContext) / (1024*1024) );
      printTime("Before build TPM");

      if((numOfTPMNode = buildTPM(log, tpm) ) >= 0) {
        printf("build TPM successful, total number nodes:%d\n", numOfTPMNode);
        printTime("Finish building TPM");
        // print_tpm_source(tpm);

#if TPM_RE_TRANSITON
        // disp_tpm_buf_source(tpm, tpmBufCtxt, 89);
#endif
#ifdef STAT
        stat(tpm);
        // benchTPMDFS(tpm);
#endif

        tpmBufCtxt = initTPMBufContext(tpm); // analyze tpm buffers (could use for HitMap)

        u32 numTPMSrcNode = 0;
        TPMNode2 **aryTPMSrcNode = getTPMSrcNode(tpmBufCtxt, &numTPMSrcNode);

        BufHitCountAry tpmBufHitCountAry = newBufHitCountAry(tpmBufCtxt->numOfBuf);
        BufHitCountAryCtxt *tpmBufHitCountAryCtxt = newBufHitCountAryCtxt(
                                                    tpmBufHitCountAry, tpmBufCtxt->numOfBuf);

        // Update 2D hit count buffer array
        OperationCtxt *octxt = createOperationCtxt(UPDATE_BUF_HIT_CNT_ARY,
                                                    tpmBufHitCountAryCtxt);

        for(int i = 0; i < numTPMSrcNode; i++) {
          // printMemNodeLit(aryTPMSrcNode[i]);
          // tpmTraverse(aryTPMSrcNode[i], tpmBufHitCountAryCtxt);
          tpmTraverse(aryTPMSrcNode[i], octxt);
        }

        printBufHitCountAry(tpmBufHitCountAry, tpmBufCtxt->numOfBuf);
        statBufHitCountArray(tpmBufHitCountAry, tpmBufCtxt->numOfBuf, 64);

        // Write propagate info (2lvl hash) to files
        octxt->ot = WRITE_2LVL_HASH;
        for(int i = 0; i < numTPMSrcNode; i++) {
          tpmTraverse(aryTPMSrcNode[i], octxt);
        }

        delBufHitCountAry(&tpmBufHitCountAry);
        delBufHitCountAryCtxt(&tpmBufHitCountAryCtxt);

        delOperationCtxt(octxt);

        delTPMSrcNode(aryTPMSrcNode);

        /* 10/2/18
         * Changed the design, no need to build HitMap any more
         */


        hitMap = buildHitMap(tpm, tpmBufCtxt);   // TODO: flag forward or reverse build
        // print_hitmap_source(hitMap);

        compHitMapStat(hitMap);
        // compReverseHitMapStat(hitMap);

        BufType bufType = HitMapBuf;
        // detectHitMapAvalanche(hitMap, tpm, bufType, bufHitCntArray, 64);
        // Due to bugs in 2D hit count array, the buffer pair given by it does
        // not include all legitimate pairs. Thus call detectHitMapAvalanche()
        // for temporary work around.

        if( (bufHitCntArray = buildBufHitCntArray(hitMap, bufType) ) != NULL) {


          // Further optimization
          // Temporary Comment for debug
          createHitMapBuftHitCnt(hitMap);   // creates IN/OUT aggregate hit count array for each HitMap buffer
          analyze_aggrgt_hitcntary(hitMap, bufType, bufHitCntArray, 64);
          printBufHitCntArray(bufHitCntArray, hitMap->hitMapBufCtxt->numOfBuf);
          // printHitMapBufHitCntAry(hitMap);

          compBufHitCntArrayStat(hitMap, bufType, bufHitCntArray, 64);      // 64 bytes
          // detectHitMapAvalanche(hitMap, tpm, bufType, bufHitCntArray, 64);  // TODO: flag forward or reverse build
          delBufHitCntArray(bufHitCntArray);

        }
        else { fprintf(stderr, "build buffer hit count array error\n"); }

        delTPMBufContext(hitMap->tpmBufCtxt);
        delHitMapBufHitCnt(hitMap);
        delHitMapBufContext(hitMap->hitMapBufCtxt);
        delHitMap(hitMap);


        // searchAllAvalancheInTPM(tpm);
        delTPM(tpm);
      }
      else { fprintf(stderr, "error build TPM\n"); }
    }
    else { fprintf(stderr, "error alloc: TPMContext\n"); }
    fclose(log);
  }
  else { fprintf(stderr, "error open log:%s\n", argv[1]); exit(1); }

  return 0;
}

