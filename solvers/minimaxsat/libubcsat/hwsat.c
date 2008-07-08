/*

      ##  ##  #####    #####   $$$$$   $$$$   $$$$$$    
      ##  ##  ##  ##  ##      $$      $$  $$    $$      
      ##  ##  #####   ##       $$$$   $$$$$$    $$      
      ##  ##  ##  ##  ##          $$  $$  $$    $$      
       ####   #####    #####  $$$$$   $$  $$    $$      
  ======================================================
  SLS SAT Solver from The University of British Columbia
  ======================================================
  ...Developed by Dave Tompkins (davet [@] cs.ubc.ca)...
  ------------------------------------------------------
  .......consult legal.txt for legal information........
  ......consult revisions.txt for revision history......
  ------------------------------------------------------
  .....e-mail ubcsat-help [@] cs.ubc.ca for support.....
  ------------------------------------------------------

*/

#include "ubcsat.h"

namespace ubcsat {

/*
unsigned int iWpNum;
unsigned int iWpDen;
*/

void AddHWSAT() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("hwsat","",0,
    "HWSAT: Gent, Walsh [Hybrid Problems... 95]",
    "Defaults,PickHWSAT,FlipVarScore");
  
  CopyParameters(pCurAlg,"hsat","",0);
  
  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","Choose random variable from an unsat clause with prob N/[M]","",&iWpNum,&iWpDen,10,100);

  AddFunction("PickHWSAT",ChooseCandidate,PickHWSAT,"VarScore,VarLastChange","");

}

void PickHWSAT() {
  unsigned int iClause;
  LITTYPE litPick;
  if (fxnRandomRatio(iWpNum,iWpDen)) {
    if (iNumFalse) {
      iClause = aFalseList[fxnRandomInt(iNumFalse)];
      litPick = pClauseLits[iClause][fxnRandomInt(aClauseLen[iClause])];
      iFlipCandidate = GetVarFromLit(litPick);
    } else {
      iFlipCandidate = 0;
    }
  } else
     PickHSAT();
}

}


