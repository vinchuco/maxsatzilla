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

void FlipVarScore2() {

  unsigned int j;
  unsigned int *pClause;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;

  if (iFlipCandidate == 0)
    return;

  aVarValue[iFlipCandidate] = 1 - aVarValue[iFlipCandidate];

  litWasFalse = GetTrueLit(iFlipCandidate);
  litWasTrue = GetFalseLit(iFlipCandidate);

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++) {
    aNumTrueLit[*pClause]--;
    pClause++;
  }

  pClause = pLitClause[litWasFalse];
  for (j=0;j<aNumLitOcc[litWasFalse];j++) {
    aNumTrueLit[*pClause]++;
    pClause++;
  }

}


void UpdateVarScore() {

  unsigned int j;
  unsigned int k;
  unsigned int *pClause;
  unsigned int iVar;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  LITTYPE *pLit;

  if (iFlipCandidate == 0)
    return;
  
  litWasFalse = GetTrueLit(iFlipCandidate);
  litWasTrue = GetFalseLit(iFlipCandidate);

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++) {
    if (aNumTrueLit[*pClause]==0) { 
      
      aFalseList[iNumFalse] = *pClause;
      aFalseListPos[*pClause] = iNumFalse++;

      aVarScore[iFlipCandidate]--;
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        aVarScore[GetVarFromLit(*pLit)]--;
        pLit++;
      }
    }
    if (aNumTrueLit[*pClause]==1) {
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        if (IsLitTrue(*pLit)) {
          iVar = GetVarFromLit(*pLit);
          aVarScore[iVar]++;
          aCritSat[*pClause] = iVar;
          break;
        }
        pLit++;
      }
    }
    pClause++;
  }

  pClause = pLitClause[litWasFalse];
  for (j=0;j<aNumLitOcc[litWasFalse];j++) {
    if (aNumTrueLit[*pClause]==1) {

      aFalseList[aFalseListPos[*pClause]] = aFalseList[--iNumFalse];
      aFalseListPos[aFalseList[iNumFalse]] = aFalseListPos[*pClause];

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aVarScore[iVar]++;
        pLit++;
      }
      aVarScore[iFlipCandidate]++;
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aVarScore[aCritSat[*pClause]]--;
    }
    *pClause++;
  }

}


void AddGSAT() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("gsat","",0,
    "GSAT: Selmen, Levesque, Mitchell [AAAI 93]",
    "Defaults,PickGSAT,FlipVarScore");
  
  AddFunction("PickGSAT",ChooseCandidate,PickGSAT,"VarScore,CandidateInfo","");


  AddFunction("FlipVarScore2",FlipCandidate,FlipVarScore2,"VarScore,FalseInfo","");
  AddFunction("UpdateVarScore",PostFlip,UpdateVarScore,"VarScore,FalseInfo","");
/*
  pCurAlg = AddAlg("gsat","test1",0,
    "GSAT: Selmen, Levesque, Mitchell [AAAI 93]",
    "Defaults,PickGSAT,FlipVarScore2,UpdateVarScore");
*/

}

void PickGSAT() {
  
  unsigned int j;
  int iScore;

  iNumCandidates = 0;
  iBestScore = iNumClause;

  for (j=1;j<=iNumVar;j++) {
    iScore = aVarScore[j];
    if (iScore == iBestScore) {
      aCandidateList[iNumCandidates++] = j;
    } else if (iScore < iBestScore) {
      *aCandidateList = j;
      iNumCandidates=1;
      iBestScore = iScore;
    }
  }
  
  if (iNumCandidates > 1)
    iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
  else
    iFlipCandidate = aCandidateList[0];
}

}
