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

ALGORITHM aAlgorithms[MAXNUMALG];
unsigned int iNumAlg = 0;

ITEMLIST listFunctions = { 0 };

FXN aFunctions[MAXITEMLIST];

FXNPTR aFxns[NUMFXNTYPES][MAXFXNLIST];
unsigned int aNumFxns[NUMFXNTYPES];
unsigned int iFunctionLoop;
unsigned int iFunctionLoop2;

/* 
  AddAlg()
  - Adds an algorithm to the algorithm list and returns pointer to it
*/

ALGORITHM *AddAlg(const char *sName, const char *sVariant, unsigned int bWeighted, 
                  const char *sDescription, 
                  const char *sFunctions) {
  
  ALGORITHM *pCurAlg;

  pCurAlg = &aAlgorithms[iNumAlg];

  SetString(&pCurAlg->sName,sName);
  pCurAlg->bWeighted = bWeighted;
  SetString(&pCurAlg->sVariant,sVariant);
  SetString(&pCurAlg->sDescription,sDescription);
  SetString(&pCurAlg->sFunctions,sFunctions);

  pCurAlg->parmList.iNumParms = 0;

  iNumAlg++;

  if (iNumAlg==MAXNUMALG) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXNUMALG [%d]\n",MAXNUMALG);
    AbnormalExit();
  }
    

  return(pCurAlg);
}

/* 
  FindAlgorithm()
  - Finds an algorithm in the table
*/

ALGORITHM *FindAlgorithm(const char *sFindName, const char *sFindVar, unsigned int bFindWeighted) {
  unsigned int j;
  if (*sFindName != 0) {
    for (j=0;j<iNumAlg;j++) {
      if (strcmp(sFindName,aAlgorithms[j].sName)==0) {
        if (strcmp(sFindVar,aAlgorithms[j].sVariant)==0) {
          if (aAlgorithms[j].bWeighted == bFindWeighted) {
            return(&aAlgorithms[j]);
          }
        }
      }
    }
  }
  return(0);
}

/* 
  CopyParameters()
  - Copies all parameters from the specified algorithm to the algorithm in pDest
*/

void CopyParameters(ALGORITHM *pDest, const char *sName, const char *sVar, unsigned int bWeighted) {
  unsigned int j;
  ALGPARMLIST *pParmList;
  ALGORITHM *pSrc = FindAlgorithm(sName,sVar,bWeighted);
  if (pSrc==0) {
    PrintString(pRepErr,"Unexpected Error: Can't find algorithm %s\n",(char *) sName);
    AbnormalExit();
  }
  pParmList = &pSrc->parmList;
  pDest->parmList.iNumParms = pParmList->iNumParms;
  for (j=0;j<pParmList->iNumParms;j++) {
    pDest->parmList.aParms[j] = pParmList->aParms[j];
  }
}

void AddFunction(const char *sID, enum FXNTYPE eFType, FXNPTR fxnFunction, char *sDependencies, char *sReplace) {
  FXN *pNewFxn;

  pNewFxn = &aFunctions[listFunctions.iNumItems];

  pNewFxn->eFType = eFType;
  pNewFxn->fxnFunction = fxnFunction;
  SetString(&pNewFxn->sDependencies,sDependencies);
  SetString(&pNewFxn->sReplace,sReplace);

  AddItem(&listFunctions,sID);
}

void CreateCompoundFunction(const char *sID, const char *sList) {
  AddCompoundItem(&listFunctions,sID,sList);
}

void DeactivateFunction(unsigned int iFxnID, const char *sItem) {
  
  unsigned int j;
  unsigned int k;
  unsigned int bAlready;
  enum FXNTYPE eFType;
  FXN *pFxn;

  NOREF(sItem);

  pFxn = &aFunctions[iFxnID];

  eFType = pFxn->eFType;

  bAlready = 0;
  for (j=0;j<aNumFxns[eFType];j++) {
    if (aFxns[eFType][j]==pFxn->fxnFunction) {
      for (k=j;k<aNumFxns[eFType]-1;k++) {
        aFxns[eFType][k] = aFxns[eFType][k+1];
      }
      aNumFxns[eFType]--;
      break;
    }
  }
}

void ActivateFunction(unsigned int iFxnID, const char *sItem) {
  
  unsigned int j;
  unsigned int bAlready;
  enum FXNTYPE eFType;
  FXN *pFxn;

  NOREF(sItem);

  pFxn = &aFunctions[iFxnID];

  ParseItemList(&listFunctions,pFxn->sDependencies,ActivateFunction);
  ParseItemList(&listFunctions,pFxn->sReplace,DeactivateFunction);

  eFType = pFxn->eFType;

  bAlready = 0;
  for (j=0;j<aNumFxns[eFType];j++) {
    if (aFxns[eFType][j]==pFxn->fxnFunction) {
      bAlready = 1;
      break;
    }
  }

  if (bAlready==0) {
    aFxns[eFType][aNumFxns[eFType]] = pFxn->fxnFunction;
    aNumFxns[eFType]++;
    if (aNumFxns[eFType] == MAXFXNLIST) {
      PrintUInt(pRepErr,"Unexpected Error: increase constant MAXFXNLIST [%d]\n",MAXFXNLIST);
      AbnormalExit();
    }
  }

}


void ActivateAlg(ALGORITHM *pAlgActivate) {
  ParseItemList(&listFunctions,pAlgActivate->sFunctions,ActivateFunction);
}

