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

void AddGWSATW();
void AddGSATW();
void AddNOVELTYW();
void AddNOVELTYPLUSW();

void AddWeightedAlgs() {

  AddGSATW();
  AddGWSATW();
  AddNOVELTYW();
  AddNOVELTYPLUSW();
}

/************************************************************************
  AddGWSATW
*************************************************************************/

FLOAT *aBreakCountW;
FLOAT *aMakeCountW;
FLOAT fFalseTotalW;

void FlipMakeBreakW() {

  unsigned int j;
  unsigned int k;
  unsigned int *pClause;
  unsigned int iVar;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  LITTYPE *pLit;

  if (iFlipCandidate == 0)
    return;

  litWasTrue = GetTrueLit(iFlipCandidate);
  litWasFalse = GetFalseLit(iFlipCandidate);

  aVarValue[iFlipCandidate] = 1 - aVarValue[iFlipCandidate];

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++) {
    aNumTrueLit[*pClause]--;
    if (aNumTrueLit[*pClause]==0) { 
      
      aFalseList[iNumFalse] = *pClause;
      aFalseListPos[*pClause] = iNumFalse++;

      fFalseTotalW += aClauseWeight[*pClause];

      aBreakCountW[iFlipCandidate] -= aClauseWeight[*pClause];
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        aMakeCountW[GetVarFromLit(*pLit)] += aClauseWeight[*pClause];
        pLit++;
      }
    }
    if (aNumTrueLit[*pClause]==1) {
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        if (IsLitTrue(*pLit)) {
          iVar = GetVarFromLit(*pLit);
          aBreakCountW[iVar] += aClauseWeight[*pClause];
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
    aNumTrueLit[*pClause]++;
    if (aNumTrueLit[*pClause]==1) {

      aFalseList[aFalseListPos[*pClause]] = aFalseList[--iNumFalse];
      aFalseListPos[aFalseList[iNumFalse]] = aFalseListPos[*pClause];

      fFalseTotalW -= aClauseWeight[*pClause];

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCountW[iVar] -= aClauseWeight[*pClause];
        pLit++;
      }
      aBreakCountW[iFlipCandidate] += aClauseWeight[*pClause];
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aBreakCountW[aCritSat[*pClause]] -= aClauseWeight[*pClause];
    }
    *pClause++;
  }
}

void CreateMakeBreakW() {
  aBreakCountW = (double*)AllocateRAM((iNumVar+1)*sizeof(FLOAT));
  aMakeCountW = (double*)AllocateRAM((iNumVar+1)*sizeof(FLOAT));
  aCritSat = (unsigned int*)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitMakeBreakW() {
  unsigned int j;
  unsigned int k;
  unsigned int iVar;
  LITTYPE *pLit;
  
  for (j=1;j<=iNumVar;j++) {
    aMakeCountW[j] = 0.0f;
    aBreakCountW[j] = 0.0f;
  }

  fFalseTotalW = 0.0f;
  
  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0) {
      for (k=0;k<aClauseLen[j];k++) {
        aMakeCountW[GetVar(j,k)] += aClauseWeight[j];
      }
      fFalseTotalW += aClauseWeight[j];
    } else if (aNumTrueLit[j]==1) {
      pLit = pClauseLits[j];
      for (k=0;k<aClauseLen[j];k++) {
        if IsLitTrue(*pLit) {
          iVar = GetVarFromLit(*pLit);
          aBreakCountW[iVar] += aClauseWeight[j];
          aCritSat[j] = iVar; /* this may cause trouble for gwsatW */
          break;
        }
        pLit++;
      }
    }
  }
}

void PickGWSATW() {
  unsigned int j;

  FLOAT fBestScore;
  FLOAT fScore;

  if (fxnRandomRatio(iWpNum,iWpDen)) {
    if (iNumFalse) {
      iFlipCandidate = aVarInFalseList[fxnRandomInt(iNumVarInFalseList)];
    } else {
      iFlipCandidate = 0;
    }
  } else {
    iNumCandidates = 0;
    fBestScore = fTotalWeight;

    for (j=1;j<=iNumVar;j++) {
      fScore = aBreakCountW[j] - aMakeCountW[j];
      if (fScore == fBestScore) {
        aCandidateList[iNumCandidates++] = j;
      } else if (fScore < fBestScore) {
        *aCandidateList = j;
        iNumCandidates=1;
        fBestScore = fScore;
      }
    }
  
    if (iNumCandidates > 1)
      iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
    else
      iFlipCandidate = aCandidateList[0];
  }
}

void FlipMakeBreakVarInFalseW() {

  unsigned int j;
  unsigned int k;
  unsigned int *pClause;
  unsigned int iVar;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  LITTYPE *pLit;

  if (iFlipCandidate == 0)
    return;

  litWasTrue = GetTrueLit(iFlipCandidate);
  litWasFalse = GetFalseLit(iFlipCandidate);

  aVarValue[iFlipCandidate] = 1 - aVarValue[iFlipCandidate];

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++) {
    aNumTrueLit[*pClause]--;
    if (aNumTrueLit[*pClause]==0) { 
      
      aFalseList[iNumFalse] = *pClause;
      aFalseListPos[*pClause] = iNumFalse++;

      fFalseTotalW += aClauseWeight[*pClause];

      aBreakCount[iFlipCandidate]--;
      aBreakCountW[iFlipCandidate] -= aClauseWeight[*pClause];
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {

        iVar = GetVarFromLit(*pLit);

        aMakeCount[iVar]++;
        aMakeCountW[iVar] += aClauseWeight[*pClause];

        if (aMakeCount[iVar]==1) {
          aVarInFalseList[iNumVarInFalseList] = iVar;
          aVarInFalseListPos[iVar] = iNumVarInFalseList++;
        }

        pLit++;
      }
    }
    if (aNumTrueLit[*pClause]==1) {
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        if (IsLitTrue(*pLit)) {
          iVar = GetVarFromLit(*pLit);
          aBreakCount[iVar]++;
          aBreakCountW[iVar] += aClauseWeight[*pClause];
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
    aNumTrueLit[*pClause]++;
    if (aNumTrueLit[*pClause]==1) {

      aFalseList[aFalseListPos[*pClause]] = aFalseList[--iNumFalse];
      aFalseListPos[aFalseList[iNumFalse]] = aFalseListPos[*pClause];
      
      fFalseTotalW -= aClauseWeight[*pClause];

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCount[iVar]--;
        aMakeCountW[iVar] -= aClauseWeight[*pClause];

        if (aMakeCount[iVar]==0) {
          aVarInFalseList[aVarInFalseListPos[iVar]] = aVarInFalseList[--iNumVarInFalseList];
          aVarInFalseListPos[aVarInFalseList[iNumVarInFalseList]] = aVarInFalseListPos[iVar];
        }

        pLit++;
      }
      aBreakCount[iFlipCandidate]++;
      aBreakCountW[iFlipCandidate] += aClauseWeight[*pClause];
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aBreakCount[aCritSat[*pClause]]--;
      aBreakCountW[aCritSat[*pClause]] -= aClauseWeight[*pClause];
    }
    *pClause++;
  }
}

void AddGWSATW() {
 
  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("gwsat","",1,
    "GWSAT: Selmen, Kautz, Cohen [AAAI 94]",
    "DefaultsW,PickGWSATW,FlipMakeBreakVarInFalseW");

  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","Choose random variable from an unsat clause with prob N/[M]","",&iWpNum,&iWpDen,50,100);

  AddFunction("PickGWSATW",ChooseCandidate,PickGWSATW,"MakeBreak,MakeBreakW,CandidateInfo,VarInFalseClause","");

  AddFunction("CreateMakeBreakW",CreateStateInfo,CreateMakeBreakW,"","");
  AddFunction("MakeBreakW",InitStateInfo,InitMakeBreakW,"CreateMakeBreakW","");
  AddFunction("FlipMakeBreakVarInFalseW",FlipCandidate,FlipMakeBreakVarInFalseW,"VarInFalseClause,FalseInfo","");

}

/************************************************************************
  AddGSATW
*************************************************************************/

void PickGSATW() {

  unsigned int j;
  
  FLOAT fBestScore;
  FLOAT fScore;

  iNumCandidates = 0;
  fBestScore = fTotalWeight;

  for (j=1;j<=iNumVar;j++) {
    fScore = aBreakCountW[j] - aMakeCountW[j];
    if (fScore == fBestScore) {
      aCandidateList[iNumCandidates++] = j;
    } else if (fScore < fBestScore) {
      *aCandidateList = j;
      iNumCandidates=1;
      fBestScore = fScore;
    }
  }

  if (iNumCandidates > 1)
    iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
  else
    iFlipCandidate = aCandidateList[0];
}

void AddGSATW() {
 
  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("gsat","",1,
    "GSAT: Selmen, Levesque, Mitchell [AAAI 93]",
    "DefaultsW,PickGSATW,FlipMakeBreakW");

  AddFunction("FlipMakeBreakW",FlipCandidate,FlipMakeBreakW,"MakeBreakW,FalseInfo","");  
  
  AddFunction("PickGSATW",ChooseCandidate,PickGSATW,"MakeBreakW,CandidateInfo","");
}


/************************************************************************
  AddNoveltyPlusW
*************************************************************************/

void PickNOVELTYWCSWE()
{
 
  unsigned int j;
  FLOAT fScore;
  unsigned int iClause = 0;
  unsigned int iClauseLen;
  LITTYPE *pLit;
  unsigned int iVar;

  unsigned int iYoungestVar;

  FLOAT fBestScore;
  FLOAT fSecondBestScore;

  FLOAT fRandClause;
  FLOAT fClauseSum;
  
  unsigned int iBestVar=0;
  unsigned int iSecondBestVar=0;

  fBestScore = fTotalWeight;
  fSecondBestScore = fTotalWeight;

  if (iNumFalse) {

    fRandClause = fxnRandomFloat() * fFalseTotalW;

    fClauseSum = 0.0f;

    for (j=0;j<iNumFalse;j++) {
      iClause = aFalseList[j];
      fClauseSum += aClauseWeight[iClause];
      if (fRandClause < fClauseSum) {
        break;
      }
    }

    iClauseLen = aClauseLen[iClause];

  } else {
    iFlipCandidate = 0;
    return;
  }

  pLit = pClauseLits[iClause];

  iYoungestVar = GetVarFromLit(*pLit);

  for (j=0;j<iClauseLen;j++) {
    
    iVar = GetVarFromLit(*pLit);
    
    fScore = aBreakCountW[iVar] - aMakeCountW[iVar];

    if (aVarLastChange[iVar] > aVarLastChange[iYoungestVar])
      iYoungestVar = iVar;

    if ((fScore < fBestScore) || ((fScore == fBestScore) && (aVarLastChange[iVar] < aVarLastChange[iBestVar]))) {
      iSecondBestVar = iBestVar;
      iBestVar = iVar;
      fSecondBestScore = fBestScore;
      fBestScore = fScore;
    } else if ((fScore < fSecondBestScore) || ((fScore == fSecondBestScore) && (aVarLastChange[iVar] < aVarLastChange[iSecondBestVar]))) {
      iSecondBestVar = iVar;
      fSecondBestScore = fScore;
    }

    pLit++;
  }
  
  iFlipCandidate = iBestVar;

  if (iFlipCandidate != iYoungestVar)
    return;

  if (fxnRandomRatio(iNovNoiseNum,iNovNoiseDen))
    iFlipCandidate = iSecondBestVar;

}

void PickNOVELTYPLUSWCSWE()
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
    PickNOVELTYWCSWE();
  }
}

void AddNOVELTYW() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("novelty","",1,
    "WALKSAT-NOVELTY: McAllester, Selmen, Kautz [AAAI 97]",
    "DefaultsW,PickNOVELTYW,FlipMakeBreakW");

  CopyParameters(pCurAlg,"novelty","",0);

  AddFunction("PickNOVELTYWCSWE",ChooseCandidate,PickNOVELTYWCSWE,"FalseInfo,VarLastChange","");

}

void AddNOVELTYPLUSW() {
  
  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("novelty+","",1,
    "WALKSAT-NOVELTY+/WCS+WE: Hoos [ref ?]",
    "Defaults,PickNOVELTYPLUSWCSWE,FlipMakeBreakW");
  
  CopyParameters(pCurAlg,"novelty+","",0);

  AddFunction("PickNOVELTYPLUSWCSWE",ChooseCandidate,PickNOVELTYPLUSWCSWE,"FalseInfo,VarLastChange","");
 
}



}
