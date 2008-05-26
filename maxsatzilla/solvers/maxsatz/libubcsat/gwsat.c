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

unsigned int iWpNum;
unsigned int iWpDen;

unsigned int iNumVarInFalseList = 0;
unsigned int *aVarInFalseList;
unsigned int *aVarInFalseListPos;


void AddGWSAT() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("gwsat","",0,
    "GWSAT: Selmen, Kautz, Cohen [AAAI 94]",
    "Defaults,PickGWSAT,FlipMakeBreakVarInFalse");

  AddParmRatio(&pCurAlg->parmList,"-wp","walk probability","Choose random variable from an unsat clause with prob N/[M]","",&iWpNum,&iWpDen,50,100);

  AddFunction("PickGWSAT",ChooseCandidate,PickGWSAT,"MakeBreak,CandidateInfo,VarInFalseClause","");

}

void PickGWSAT() {
  unsigned int j;
  int iScore;

  if (fxnRandomRatio(iWpNum,iWpDen)) {
    if (iNumFalse) {
      iFlipCandidate = aVarInFalseList[fxnRandomInt(iNumVarInFalseList)];
    } else {
      iFlipCandidate = 0;
    }
  } else {
    iNumCandidates = 0;
    iBestScore = iNumClause;

    for (j=1;j<=iNumVar;j++) {
      iScore = aBreakCount[j] - aMakeCount[j];
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

