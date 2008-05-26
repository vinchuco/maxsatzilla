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

unsigned int iNovNoiseNum;
unsigned int iNovNoiseDen;

void AddNOVELTY() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("novelty","",0,
    "WALKSAT-NOVELTY: McAllester, Selmen, Kautz [AAAI 97]",
    "Defaults,PickNOVELTY,FlipFalseInfo");
  
  
  AddParmRatio(&pCurAlg->parmList,"-novnoise","Novelty Noise","Novety noise set to prob N/[M]","",&iNovNoiseNum,&iNovNoiseDen,50,100);

  AddFunction("PickNOVELTY",ChooseCandidate,PickNOVELTY,"FalseInfo,VarLastChange","");

}

void AddNOVELTYPLUS() {
  
  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("novelty+","",0,
    "WALKSAT-NOVELTY+: Hoos [AAAI 99]",
    "Defaults,PickNOVELTYPLUS,FlipFalseInfo");
  
  CopyParameters(pCurAlg,"novelty","",0);

  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","Choose random variable from random false clause with prob N/[M]","",&iWpNum,&iWpDen,1,100);

  AddFunction("PickNOVELTYPLUS",ChooseCandidate,PickNOVELTYPLUS,"FalseInfo,VarLastChange","");
 
}

void PickNOVELTY() {
 
  unsigned int i;
  unsigned int j;
  signed int iScore;
  unsigned int iClause;
  unsigned int iClauseLen;
  LITTYPE *pLit;
  LITTYPE *pClause;
  unsigned int iNumOcc;
  unsigned int iVar;

  unsigned int iYoungestVar;

  signed int iSecondBestScore;
  
  unsigned int iBestVar=0;
  unsigned int iSecondBestVar=0;

  iBestScore = iNumClause+1;
  iSecondBestScore = iNumClause+1;

  if (iNumFalse) {
    iClause = aFalseList[fxnRandomInt(iNumFalse)];
    iClauseLen = aClauseLen[iClause];
  } else {
    iFlipCandidate = 0;
    return;
  }

  pLit = pClauseLits[iClause];

  iYoungestVar = GetVarFromLit(*pLit);

  for (j=0;j<iClauseLen;j++) {
    iScore = 0;

    iVar = GetVarFromLit(*pLit);

    iNumOcc = aNumLitOcc[*pLit];
    pClause = pLitClause[*pLit];
    
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause++]==0) {
        iScore--;
      }
    }

    iNumOcc = aNumLitOcc[GetNegatedLit(*pLit)];
    pClause = pLitClause[GetNegatedLit(*pLit)];
    
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause++]==1) {
        iScore++;
      }
    }

    if (aVarLastChange[iVar] > aVarLastChange[iYoungestVar])
      iYoungestVar = iVar;

    if ((iScore < iBestScore) || ((iScore == iBestScore) && (aVarLastChange[iVar] < aVarLastChange[iBestVar]))) {
      iSecondBestVar = iBestVar;
      iBestVar = iVar;
      iSecondBestScore = iBestScore;
      iBestScore = iScore;
    } else if ((iScore < iSecondBestScore) || ((iScore == iSecondBestScore) && (aVarLastChange[iVar] < aVarLastChange[iSecondBestVar]))) {
      iSecondBestVar = iVar;
      iSecondBestScore = iScore;
    }

    pLit++;
  }
  
  iFlipCandidate = iBestVar;

  if (iFlipCandidate != iYoungestVar)
    return;

  if (fxnRandomRatio(iNovNoiseNum,iNovNoiseDen))
    iFlipCandidate = iSecondBestVar;


}

void PickNOVELTYPLUS()
{
 
  unsigned int iClause;
  unsigned int iClauseLen;

  LITTYPE litPick;
  if (fxnRandomRatio(iWpNum,iWpDen)) {
    if (iNumFalse) {
      iClause = aFalseList[fxnRandomInt(iNumFalse)];
      iClauseLen = aClauseLen[iClause];
      litPick = (pClauseLits[iClause][fxnRandomInt(iClauseLen)]);
      iFlipCandidate = GetVarFromLit(litPick);
    } else {
      iFlipCandidate = 0;
    }
  } else {
    PickNOVELTY();
  }
}


}
