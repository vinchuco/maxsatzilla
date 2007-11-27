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

FLOAT fAlpha;
FLOAT fRho;
FLOAT fPenaltyImprove;
unsigned int iPsNum;
unsigned int iPsDen;

FLOAT *aClausePenalty;
FLOAT fPenaltySum;
FLOAT *aMakePenalty;
FLOAT *aBreakPenalty;
unsigned int bClausePenaltyCreated = 0;

FLOAT fMaxClausePenalty = 1000;

void AddSAPS() {

  bClausePenaltyCreated = 0;
  fMaxClausePenalty = 1000;

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("saps","",0,
    "SAPS: Hutter, Tompkins, Hoos [CP 02]",
    "Defaults,PickSAPS,FlipSAPS,PostFlipSAPS");
  
  AddParmFloat(&pCurAlg->parmList,"-alpha","alpha","Algorithm scaling parameter alpha","",&fAlpha,1.3f);
  AddParmFloat(&pCurAlg->parmList,"-rho","rho","Algorithm smoothing parameter rho","",&fRho,0.8f);
  AddParmRatio(&pCurAlg->parmList,"-ps","smooth probabilty","When a local minimum is encountered, Smooth Penalties with prob. N/M","",&iPsNum,&iPsDen,5,100);
  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","When a local minimum is encountered, walk with prob. N/M","",&iWpNum,&iWpDen,1,100);
  AddParmFloat(&pCurAlg->parmList,"-sapsthresh","SAPS Threshold for Improvement","Sets threshold for detecting a local minimum","",&fPenaltyImprove,-1.0e-01f);

  AddFunction("CreateClausePenalty",CreateStateInfo,CreateClausePenalty,"CreateMakeBreak","");
  AddFunction("ClausePenalty",InitStateInfo,InitClausePenalty,"CreateClausePenalty,MakeBreak","");

  AddFunction("PickSAPS",ChooseCandidate,PickSAPS,"ClausePenalty,FalseInfo,MakeBreak,VarInFalseClause,CandidateInfo","");
  AddFunction("FlipSAPS",FlipCandidate,FlipSAPS,"ClausePenalty,FalseInfo,MakeBreak,VarInFalseClause","");
  AddFunction("PostFlipSAPS",PostFlip,PostFlipSAPS,"","");

}

void CreateClausePenalty() {
  aClausePenalty =(double*) AllocateRAM(iNumClause * sizeof(FLOAT));
  aBreakPenalty =(double*)AllocateRAM((iNumVar+1)*sizeof(FLOAT));
  aMakePenalty =(double*) AllocateRAM((iNumVar+1)*sizeof(FLOAT));
  bClausePenaltyCreated = 1;
}

void InitMakeBreakPenalty() {

  unsigned int j;
  unsigned int k;
  unsigned int iVar;
  LITTYPE *pLit;

  for (j=1;j<=iNumVar;j++) {
    aMakePenalty[j] = 0.0f;
    aBreakPenalty[j] = 0.0f;
  }

  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0) {
      for (k=0;k<aClauseLen[j];k++) {
        aMakePenalty[GetVar(j,k)] += aClausePenalty[j];
      }
    } else if (aNumTrueLit[j]==1) {
      pLit = pClauseLits[j];
      for (k=0;k<aClauseLen[j];k++) {
        if IsLitTrue(*pLit) {
          iVar = GetVarFromLit(*pLit);
          aBreakPenalty[iVar] += aClausePenalty[j];
          aCritSat[j] = iVar;
          break;
        }
        pLit++;
      }
    }
  }
}

void InitClausePenalty() {
  unsigned int j;
  
  for (j=0;j<iNumClause;j++) {
    aClausePenalty[j] = 1.0f;
  }

  fPenaltySum = (FLOAT) iNumClause;

  for (j=1;j<=iNumVar;j++) {
    aMakePenalty[j] = (FLOAT) aMakeCount[j];
    aBreakPenalty[j] = (FLOAT) aBreakCount[j];
  }

}

void PickSAPS() {
  
  unsigned int j;
  unsigned int iVar;
  FLOAT fScore;
  FLOAT fBestScore;

  iNumCandidates = 0;
  fBestScore = 1000000.0f;

  for (j=0;j<iNumVarInFalseList;j++) {
    iVar = aVarInFalseList[j];
    fScore = aBreakPenalty[iVar] - aMakePenalty[iVar];
    if (fScore <= fBestScore) {
      if (fScore < fBestScore) {
        iNumCandidates = 0;
      }
      fBestScore = fScore;
      aCandidateList[iNumCandidates++] = iVar;
    }
  }

  if (fBestScore >= fPenaltyImprove) {
    if (fxnRandomRatio(iWpNum,iWpDen)) {
      iFlipCandidate = fxnRandomInt(iNumVar) + 1;
    } else {
      iFlipCandidate = 0;
    }
  } else {
    if (iNumCandidates > 1) {
      iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
    } else {
      iFlipCandidate = aCandidateList[0];
    }
  }
}

void FlipSAPS() {

  unsigned int j;
  unsigned int k;
  unsigned int *pClause;
  unsigned int iVar;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  LITTYPE *pLit;

  FLOAT fPenalty;

  if (iFlipCandidate == 0)
    return;

  litWasTrue = GetTrueLit(iFlipCandidate);
  litWasFalse = GetFalseLit(iFlipCandidate);

  aVarValue[iFlipCandidate] = 1 - aVarValue[iFlipCandidate];

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++) {
    fPenalty = aClausePenalty[*pClause];
    aNumTrueLit[*pClause]--;
    if (aNumTrueLit[*pClause]==0) { 
      
      aFalseList[iNumFalse] = *pClause;
      aFalseListPos[*pClause] = iNumFalse++;

      aBreakCount[iFlipCandidate]--;
      
      aBreakPenalty[iFlipCandidate] -= fPenalty;
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCount[iVar]++;
        aMakePenalty[iVar] += fPenalty;

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
          aBreakPenalty[iVar] += fPenalty;
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
    fPenalty = aClausePenalty[*pClause];
    aNumTrueLit[*pClause]++;
    if (aNumTrueLit[*pClause]==1) {

      aFalseList[aFalseListPos[*pClause]] = aFalseList[--iNumFalse];
      aFalseListPos[aFalseList[iNumFalse]] = aFalseListPos[*pClause];

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCount[iVar]--;
        aMakePenalty[iVar] -= fPenalty;

        if (aMakeCount[iVar]==0) {
          aVarInFalseList[aVarInFalseListPos[iVar]] = aVarInFalseList[--iNumVarInFalseList];
          aVarInFalseListPos[aVarInFalseList[iNumVarInFalseList]] = aVarInFalseListPos[iVar];
        }
        
        pLit++;

      }
      aBreakCount[iFlipCandidate]++;
      aBreakPenalty[iFlipCandidate] += fPenalty;
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aBreakCount[aCritSat[*pClause]]--;
      aBreakPenalty[aCritSat[*pClause]] -= fPenalty;
    }
    *pClause++;
  }
}

void SmoothSAPS() {
  
  FLOAT fAveragePenalty;
  unsigned int j;
 

  fAveragePenalty = fPenaltySum / (FLOAT) iNumClause;
  fAveragePenalty *= (1.0f - fRho);

  for(j=0;j<iNumClause;j++) {
    aClausePenalty[j] += fAveragePenalty;
  }
    
  fPenaltySum += (fAveragePenalty * (FLOAT) iNumClause);

  for (j=1;j<=iNumVar;j++) {
    aMakePenalty[j] += fAveragePenalty * (FLOAT) aMakeCount[j];
    aBreakPenalty[j] += fAveragePenalty * (FLOAT) aBreakCount[j];
  }

}


void AdjustPenalties() {
  
  unsigned int j;
  unsigned int k;
  unsigned int bReScale = 0;
  FLOAT fDiff;
  FLOAT fOld;
  LITTYPE *pLit;

  for(j=0;j<iNumFalse;j++) {
    if (aClausePenalty[aFalseList[j]] > fMaxClausePenalty) {
      bReScale = 1;
      break;
    }
  }
  
  if (bReScale) {

    fPenaltySum = 0;

    for(j=0;j<iNumClause;j++) {

      fOld = aClausePenalty[j];
      aClausePenalty[j] /= fMaxClausePenalty;
      fDiff = aClausePenalty[j] - fOld;

      if (aNumTrueLit[j]==0) {
        pLit = pClauseLits[j];
        for (k=0;k<aClauseLen[j];k++) {
          aMakePenalty[GetVarFromLit(*pLit)] += fDiff;
          pLit++;
        }
      }

      if (aNumTrueLit[j]==1) {
        aBreakPenalty[aCritSat[j]] += fDiff;
      }

      fPenaltySum += aClausePenalty[j];
    }
  }

}

void ScaleSAPS() {
  unsigned int j;
  unsigned int k;
  unsigned int iClause;
  FLOAT fOld;
  FLOAT fDiff;
  LITTYPE *pLit;

  for(j=0;j<iNumFalse;j++) {
    
    iClause = aFalseList[j];

    fOld = aClausePenalty[iClause];

    aClausePenalty[iClause] *= fAlpha;

    fDiff = aClausePenalty[iClause] - fOld;

    pLit = pClauseLits[iClause];
    for (k=0;k<aClauseLen[iClause];k++) {
      aMakePenalty[GetVarFromLit(*pLit)] += fDiff;
      pLit++;
    }
		fPenaltySum += fDiff;
  }
}


void PostFlipSAPS() {
  if (iFlipCandidate)
    return;

  if (fxnRandomRatio(iPsNum,iPsDen)) {
    SmoothSAPS();
  }

  AdjustPenalties();
  
  ScaleSAPS();

}

