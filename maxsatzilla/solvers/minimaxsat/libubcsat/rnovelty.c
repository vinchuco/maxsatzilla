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

void AddRNOVELTY() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("rnovelty","",0,
    "WALKSAT-RNOVELTY: McAllester, Selmen, Kautz [AAAI 97]",
    "Defaults,PickRNOVELTY,FlipFalseInfo");
  
  CopyParameters(pCurAlg,"novelty","",0);

  AddFunction("PickRNOVELTY",ChooseCandidate,PickRNOVELTY,"FalseInfo,VarLastChange","");

}

void AddRNOVELTYPLUS() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("rnovelty+","",0,
    "WALKSAT-RNOVELTY+: Hoos [AAAI 99]",
    "Defaults,PickRNOVELTYPLUS,FlipFalseInfo");
    
  CopyParameters(pCurAlg,"novelty+","",0);

  AddFunction("PickRNOVELTYPLUS",ChooseCandidate,PickRNOVELTYPLUS,"FalseInfo,VarLastChange","");

}


void PickRNOVELTYCore()
{
 
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
  signed int iScoreMargin;
  
  unsigned int iBestVar=0;
  unsigned int iSecondBestVar=0;

  unsigned int iNovNoiseDenDiv2 = iNovNoiseDen >> 1;


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

  iScoreMargin = iSecondBestScore - iBestScore;

  if ((iNovNoiseNum < iNovNoiseDenDiv2)) {
    if (iScoreMargin > 1)
      return;
    if (iScoreMargin == 1) {
      if (fxnRandomRatio(iNovNoiseNum << 1,iNovNoiseDen))
        iFlipCandidate = iSecondBestVar;
      return;
    }
  }
  if (iScoreMargin == 1) {
    iFlipCandidate = iSecondBestVar;
    return;
  } 

  if (fxnRandomRatio(((iNovNoiseNum - iNovNoiseDenDiv2)<<1),iNovNoiseDen))
    iFlipCandidate = iSecondBestVar;
}

void PickRNOVELTY()
{
 
  unsigned int iClause;
  unsigned int iClauseLen;
  LITTYPE litPick;

  if ((iStep % 100) == 0) {
    if (iNumFalse) {
      iClause = aFalseList[fxnRandomInt(iNumFalse)];
      iClauseLen = aClauseLen[iClause];
      litPick = (pClauseLits[iClause][fxnRandomInt(iClauseLen)]);
      iFlipCandidate = GetVarFromLit(litPick);
    } else {
      iFlipCandidate = 0;
    }
    return;
  }
  
  PickRNOVELTYCore();
}


void PickRNOVELTYPLUS()
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
    return;
  }

  PickRNOVELTYCore();

}

}
