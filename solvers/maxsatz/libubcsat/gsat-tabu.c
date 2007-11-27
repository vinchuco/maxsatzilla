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

unsigned int iTabuTenure=0;

void AddGSATTABU() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("gsat-tabu","",0,
    "GSAT-TABU: Mazure, Sais, Gregoire [CP 95]",
    "Defaults,PickGSATTABU,FlipVarScore");
  
  AddParmInt(&pCurAlg->parmList,"-tabu","tabu tenure","variables flipped within the last N steps are tabu","",&iTabuTenure,10);

  AddFunction("PickGSATTABU",ChooseCandidate,PickGSATTABU,"VarScore,CandidateInfo,VarLastChange","");

}

void PickGSATTABU() {
  
  unsigned int j;
  int iScore;
  signed int iTabuCutoff;

  iTabuCutoff = (signed int) iStep - (signed int) iTabuTenure;

  iNumCandidates = 0;
  iBestScore = iNumClause;

  for (j=1;j<=iNumVar;j++) {
    if (aVarLastChange[j] < iTabuCutoff) { 
      iScore = aVarScore[j];
      if (iScore == iBestScore) {
        aCandidateList[iNumCandidates++] = j;
      } else if (iScore < iBestScore) {
        *aCandidateList = j;
        iNumCandidates=1;
        iBestScore = iScore;
      }
    }
  }

  if (iNumCandidates > 1)
    iFlipCandidate = aCandidateList[fxnRandomInt(iNumCandidates)];
  else
    iFlipCandidate = aCandidateList[0];

}

