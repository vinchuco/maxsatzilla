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

void AddHSAT() {

  ALGORITHM *pCurAlg;

  pCurAlg = AddAlg("hsat","",0,
    "HSAT: Gent, Walsh [AAAI 93]",
    "Defaults,PickHSAT,FlipVarScore");
  
  AddFunction("PickHSAT",ChooseCandidate,PickHSAT,"VarScore,VarLastChange","");
 
}

void PickHSAT() {
  
  unsigned int j;
  int iScore;

  iNumCandidates = 0;
  iBestScore = iNumClause;

  for (j=1;j<=iNumVar;j++) {
    iScore = aVarScore[j];
    if (iScore == iBestScore) {
      if (aVarLastChange[j]==aVarLastChange[*aCandidateList]) {
        aCandidateList[iNumCandidates++] = j;
      } else if (aVarLastChange[j]<aVarLastChange[*aCandidateList]) {
        *aCandidateList = j;
        iNumCandidates=1;
        iBestScore = iScore;
      }
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
