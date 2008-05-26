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

void AddWALKSATTABU() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("walksat-tabu","",0,
    "WALKSAT-TABU: McAllester, Selmen, Kautz [AAAI 97]",
    "Defaults,PickWALKSATTABU,FlipFalseInfo");
  
  AddParmInt(&pCurAlg->parmList,"-tabu","tabu tenure","variables flipped within the last N steps are tabu","",&iTabuTenure,10);

  AddFunction("PickWALKSATTABU",ChooseCandidate,PickWALKSATTABU,"FalseInfo,VarLastChange","");
  
}

unsigned int iWalkSATTabuClause;

void PickWALKSATTABU()
{
 
  unsigned int i;
  unsigned int j;
  int iScore;
  unsigned int iClauseLen;
  LITTYPE *pLit;
  LITTYPE *pClause;
  unsigned int iNumOcc;
  unsigned int iVar;

  iNumCandidates = 0;
  iBestScore = iNumClause+1;

  if (iNumFalse) {
    iWalkSATTabuClause = aFalseList[fxnRandomInt(iNumFalse)];
    iClauseLen = aClauseLen[iWalkSATTabuClause];
  } else {
    iFlipCandidate = 0;
    return;
  }


  pLit = pClauseLits[iWalkSATTabuClause];

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

    if ((iScore==0)||(aVarLastChange[iVar] + (signed int) iTabuTenure < (signed int) iStep)) { 
      if (iScore == iBestScore) {
        aCandidateList[iNumCandidates++] = iVar;
      } else if (iScore < iBestScore) {
        *aCandidateList = iVar;
        iNumCandidates=1;
        iBestScore = iScore;
      }
    }
    pLit++;
  }

  if (iNumCandidates == 0) {
    iFlipCandidate = 0;
    return;
  }

    if (iNumCandidates > 1)
      iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
    else
      iFlipCandidate = aCandidateList[0];

}

