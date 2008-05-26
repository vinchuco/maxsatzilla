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

void AddWALKSAT() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("walksat","",0,
    "WALKSAT (SKC Variant): Selmen, Kautz, Cohen [AAAI 94]",
    "Defaults,PickWALKSATSKG,FlipFalseInfo");
  
  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","Choose random variable from random false clause with prob N/[M]","",&iWpNum,&iWpDen,50,100);

  AddFunction("PickWALKSATSKG",ChooseCandidate,PickWALKSATSKG,"FalseInfo","");
 
}

void PickWALKSATSKG()
{
 
  unsigned int i;
  unsigned int j;
  int iScore;
  unsigned int iClause;
  unsigned int iClauseLen;
  unsigned int iVar;
  LITTYPE *pLit;
  LITTYPE *pClause;
  LITTYPE litPick;
  unsigned int iNumOcc;

  iNumCandidates = 0;
  iBestScore = iNumClause;

  if (iNumFalse) {
    iClause = aFalseList[fxnRandomInt(iNumFalse)];
    iClauseLen = aClauseLen[iClause];
  } else {
    iFlipCandidate = 0;
    return;
  }


  pLit = pClauseLits[iClause];

  for (j=0;j<iClauseLen;j++) {
    iScore = 0;
    
    iVar = GetVarFromLit(*pLit);
    
    iNumOcc = aNumLitOcc[GetNegatedLit(*pLit)];
    pClause = pLitClause[GetNegatedLit(*pLit)];
    
    for (i=0;i<iNumOcc;i++) {
      if (aNumTrueLit[*pClause++]==1) {
        iScore++;
      }
    }

    if (iScore == iBestScore) {
      aCandidateList[iNumCandidates++] = iVar;
    } else if (iScore < iBestScore) {
      *aCandidateList = iVar;
      iNumCandidates=1;
      iBestScore = iScore;
    }
    pLit++;
  }

  if (iBestScore > 0) {
    if (fxnRandomRatio(iWpNum,iWpDen)) {
      litPick = pClauseLits[iClause][fxnRandomInt(iClauseLen)];
      iFlipCandidate = GetVarFromLit(litPick);
      return;
    }
  }

  if (iNumCandidates > 1)
    iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
  else
    iFlipCandidate = aCandidateList[0];

}



