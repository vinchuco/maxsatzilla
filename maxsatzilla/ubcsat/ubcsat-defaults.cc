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

/* 
  - A lot of the variables used by UBCSAT are delcared here
  - In a future release of ubcsat more documentation will be provided
*/

char sNull = 0;
unsigned int bNull = 0;

char *sAlgName = &sNull;
char *sVarName = &sNull;
unsigned int bWeighted = 0;

unsigned int bShowHelp;
unsigned int bShowHelpA;
unsigned int bShowHelpV;
unsigned int bShowHelpT;
unsigned int bShowHelpR;
unsigned int bShowHelpC;
unsigned int bShowHelpS;

unsigned int iNumRuns;
unsigned int iCutoff;
unsigned int iSeed;
unsigned int iTarget;
unsigned int iFind;
unsigned int iNumFound=0;
unsigned int iRestart = 0;

unsigned int iRun = 0;
unsigned int iStep;

unsigned int bTerminateAllRuns = 0;
unsigned int bSolutionFound = 0;
unsigned int bTerminateRun = 0;

unsigned int bSolveMode = 0;
unsigned int bSatzillaMode = 0;

char *sFilenameIn = &sNull;
char *sFilenameParms;
char *sFilenameVarInit = &sNull;

ALGORITHM *pDefaultAlg;

ALGPARMLIST parmUBCSAT;
ALGPARMLIST parmHelp;
ALGPARMLIST parmIO;
ALGPARMLIST parmAlg;

unsigned int bReportEcho;
unsigned int bReportClean;

unsigned int *aNumLitOcc;
unsigned int *aLitOccData;
unsigned int **pLitClause;

unsigned int iNumFalse;
unsigned int *aFalseList;
unsigned int *aFalseListPos;
unsigned int *aNumTrueLit;
unsigned int *aVarValue;
unsigned int *aVarInit;

unsigned int *aBreakCount;
unsigned int *aMakeCount;

unsigned int *aCritSat;

signed int *aVarScore;

unsigned int iBestNumFalse;
unsigned int iBestStepNumFalse;

FLOAT fBestFalseWeight;
unsigned int iBestStepFalseWeight;

unsigned int iNoImprove=0;
FLOAT fNoImproveFactor;
FLOAT fTimeout=0;

signed int *aVarLastChange;

FLOAT fFalseWeight;

unsigned int iFlipCandidate;
unsigned int *aCandidateList;
unsigned int iNumCandidates;
int iBestScore;

unsigned int iStartSeed;

unsigned int iInitVarFlip = 0;

unsigned int iNumNullFlips;


/* 
  AddDEFAULT()
  - Sets up the default algorithm
*/

void AddDEFAULT() {

  AddFunction("ReadCNF",ReadInFile,ReadCNF,"","");

  AddFunction("LitOccurence",CreateStructure,CreateLitOccurence,"","");
  
  AddFunction("CreateMinimalStateInfo",CreateStateInfo,CreateMinimalStateInfo,"","");
  AddFunction("MinimalStateInfo",InitStateInfo,InitMinimalStateInfo,"CreateMinimalStateInfo","");

  AddFunction("CandidateInfo",CreateStateInfo,CreateCandidateInfo,"","");

  AddFunction("InitVarFile",CreateStateInfo,InitVarFile,"MinimalStateInfo","");

  AddFunction("InitVars",InitData,InitVars,"MinimalStateInfo","");

  AddFunction("CheckTermination",CheckTerminate,CheckTermination,"","");

  CreateCompoundFunction("Defaults","ReadCNF,InitVarFile,LitOccurence,MinimalStateInfo,CandidateInfo,InitVars,CheckTermination");

  AddFunction("CheckTerminationW",CheckTerminate,CheckTerminationW,"","");
  AddFunction("MinimalStateInfoW",InitStateInfo,InitMinimalStateInfoW,"MinimalStateInfo","");
  CreateCompoundFunction("DefaultsW","ReadCNF,InitVarFile,LitOccurence,MinimalStateInfoW,CandidateInfo,InitVars,CheckTerminationW");

  AddFunction("FlipMinimal",FlipCandidate,FlipMinimal,"","");
  AddFunction("FlipFalseInfo",FlipCandidate,FlipFalseInfo,"FalseInfo","");

  AddFunction("CreateFalseInfo",CreateStateInfo,CreateFalseInfo,"","");
  AddFunction("FalseInfo",InitStateInfo,InitFalseInfo,"CreateFalseInfo","");

  AddFunction("CreateMakeBreak",CreateStateInfo,CreateMakeBreak,"","");
  AddFunction("MakeBreak",InitStateInfo,InitMakeBreak,"CreateMakeBreak","");
  AddFunction("FlipMakeBreak",FlipCandidate,FlipMakeBreak,"MakeBreak,FalseInfo","");

  AddFunction("CreateVarScore",CreateStateInfo,CreateVarScore,"","");
  AddFunction("VarScore",InitStateInfo,InitVarScore,"CreateVarScore","");
  AddFunction("FlipVarScore",FlipCandidate,FlipVarScore,"VarScore,FalseInfo","");

  AddFunction("CreateVarInFalseClause",CreateStateInfo,CreateVarInFalseClause,"","");
  AddFunction("VarInFalseClause",InitStateInfo,InitVarInFalseClause,"CreateVarInFalseClause","");

  AddFunction("FlipMakeBreakVarInFalse",FlipCandidate,FlipMakeBreakVarInFalse,"VarInFalseClause,FalseInfo","");

  AddFunction("CreateVarLastChange",CreateStateInfo,CreateVarLastChange,"","");
  AddFunction("InitVarLastChange",InitStateInfo,InitVarLastChange,"CreateVarLastChange","");
  AddFunction("VarLastChange",PostFlip,UpdateVarLastChange,"InitVarLastChange","");

  AddFunction("InitNullFlips",PreRun,InitNullFlips,"","");
  AddFunction("NullFlips",PostFlip,UpdateNullFlips,"InitNullFlips","");

}

/* 
  BuildParameters()
  - Sets up the paramaters common to all algorithms
*/

void BuildParameters() {
  AddParmString(&parmAlg,"-alg","algorithm","The name of the algorithm","",&sAlgName,&sNull);
  AddParmString(&parmAlg,"-v","variant","The algorithm variant (if any)","",&sVarName,&sNull);

  /* 
  This option has been removed for the version 1.0 release to avoid confusion
  */ 
  AddParmBool(&parmAlg,"-w","weighted","The algorithm uses weighted clauses","",&bWeighted,0);

  AddParmBool(&parmHelp,"-h|-help|--help","general help","Display general help and algorithm help if specified","",&bShowHelp,0);
  AddParmBool(&parmHelp,"-ha|-helpalg","algorithm help","Display list of all algorithms","",&bShowHelpA,0);
  AddParmBool(&parmHelp,"-hv|-helpverbose","verbose help","Display (verbose) help for all algorithms","",&bShowHelpV,0);
  AddParmBool(&parmHelp,"-ht|-helpterse","terse help","Display (terse) help for all algorithms","",&bShowHelpT,0);
  AddParmBool(&parmHelp,"-hr|-helpreports","report help","Display list of all reports","",&bShowHelpR,0);
  AddParmBool(&parmHelp,"-hc|-helpcolumns","report columns help","Display help for report columns","",&bShowHelpC,0);
  AddParmBool(&parmHelp,"-hs|-helpstats","statistics report help","Display help for the statistics reports","",&bShowHelpS,0);

  AddParmInt(&parmUBCSAT,"-runs","runs","The number of attempts (runs)","",&iNumRuns,1);
  AddParmInt(&parmUBCSAT,"-cutoff","cutoff","The maximum number of search steps per run","",&iCutoff,300000);
  AddParmInt(&parmUBCSAT,"-target","target","Solution found if [number of / cost of] false clauses <= target (MAXSAT)","",&iTarget,0);
  AddParmInt(&parmUBCSAT,"-seed","seed","Force a specific random seed","",&iSeed,iSeed);
  AddParmBool(&parmUBCSAT,"-solve","solve mode","Terminate when a solution has been found and output solution","SolveMode",&bSolveMode,0);
  AddParmBool(&parmUBCSAT,"-satzilla","satzilla mode","Terminate when a solution has been found and output solution, also write satzilla features","SolveMode",&bSatzillaMode,0);
  AddParmInt(&parmUBCSAT,"-find|-numsol","find N solutions","Terminate when N solutions have been found","",&iFind,0);
  AddParmInt(&parmUBCSAT,"-restart","restart","Reinitialize variables every N steps","",&iRestart,0);

  /*  AddParmInt(&parmUBCSAT,"-noimprove","no improve","Terminate run if no improvement in N steps","NoImprove",&iNoImprove,0);*/
  AddParmFloat(&parmUBCSAT,"-noimprove","no improve","Terminate run if no improvement in nVars*factor steps","NoImprove",&fNoImproveFactor,0);
  
  AddParmFloat(&parmUBCSAT, "-timeout", "time out", "Terminate all runs once time exceeds timeout", "CheckTermination", &fTimeout, 0);

  AddParmString(&parmIO,"-inst|-i","input instance file","Specify an input instance file (default is stdin)","",&sFilenameIn,"");

  AddParmString(&parmIO,"-varinit","variable initialisation file","Variable initialisation file format = -1 3 -4 ...","",&sFilenameVarInit,"");
  AddParmInt(&parmIO,"-varinitflip","flip after initialisation","Force N variables to be flipped after initialisation","CandidateInfo",&iInitVarFlip,0);

  AddParmString(&parmIO,"-param|-fp","parameter file","Specify a parameter file (default is command line)","",&sFilenameParms,"");

  AddParmReport(&parmIO,"-report|-r","report(s)","Specify reports to run: use -hr for more info","");

  AddParmBool(&parmIO, "-recho","echo to screen","Echo all file output to screen as well","",&bReportEcho,0);
  AddParmBool(&parmIO, "-rclean","clean output","Supress all header output","",&bReportClean,0);

  AddParmString(&parmIO, "-rcomment","comment character","Change the header / comment string (# is default)","",&sCommentString,"#");

}

void CreateLitOccurence() {

  unsigned int j,k;
  LITTYPE *pLit;
  LITTYPE *pCur;

  aNumLitOcc = (unsigned int *)AllocateRAM((iNumVar+1)*2*sizeof(unsigned int));
  pLitClause = (unsigned int **)AllocateRAM((iNumVar+1)*2*sizeof(unsigned int *));
  aLitOccData = (unsigned int *)AllocateRAM(iNumLit*sizeof(unsigned int));

  memset(aNumLitOcc,0,(iNumVar+1)*2*sizeof(unsigned int));
  
  for (j=0;j<iNumClause;j++) {
    pLit = pClauseLits[j];
    for (k=0;k<aClauseLen[j];k++) {
      aNumLitOcc[*pLit]++;
      pLit++;
    }
  } 
  
  pCur = aLitOccData;
  for (j=0;j<(iNumVar+1)*2;j++) {
    pLitClause[j] = pCur;
    pCur += aNumLitOcc[j];
  }

  memset(aNumLitOcc,0,(iNumVar+1)*2*sizeof(unsigned int));

  for (j=0;j<iNumClause;j++) {
    pLit = pClauseLits[j];
    for (k=0;k<aClauseLen[j];k++) {
      pCur = pLitClause[*pLit] + aNumLitOcc[*pLit];
      *pCur=j;
      aNumLitOcc[*pLit]++;
      pLit++;
    }
  }
}

void CreateCandidateInfo() {
  aCandidateList = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void CreateMinimalStateInfo() {
  aNumTrueLit = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
  aVarValue = (unsigned int *)AllocateRAM((iNumVar+1)*sizeof(unsigned int));
  aVarInit = (unsigned int *)AllocateRAM((iNumVar+1)*sizeof(unsigned int));
}

/* 
  InitMinimalStateInfo()
  - StateInfo info has to be initialized at the start of each run
*/

void InitMinimalStateInfo() {
  unsigned int j,k;
  LITTYPE litCur;
  unsigned int *pClause;
  
  memset(aNumTrueLit,0,iNumClause*sizeof(unsigned int));
  iNumFalse = 0;

  for (j=1;j<=iNumVar;j++) {
    litCur = GetTrueLit(j);
    pClause = pLitClause[litCur];
    for (k=0;k<aNumLitOcc[litCur];k++) {
      aNumTrueLit[*pClause]++;
      pClause++;
      }
    }

  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0)
      iNumFalse++;
  }

}

void InitMinimalStateInfoW() {
  unsigned int j;
  
  fFalseWeight = 0.0f;

  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0)
      fFalseWeight += aClauseWeight[j];
  }

}




/* 
  CheckBestFalse()
  - Keeps track of the best solution quality found so far
*/

void CheckBestFalse() {
  if (iNumFalse < iBestNumFalse) {
    iBestNumFalse = iNumFalse;
    iBestStepNumFalse = iStep;
  }
  if (bWeighted) {
    if (fFalseWeight <= fBestFalseWeight) {
      fBestFalseWeight = fFalseWeight;
      iBestStepFalseWeight = iStep;
    }
  }
}

void InitBestFalse() {
  iBestNumFalse = iNumClause+1;
  iBestStepNumFalse = 0; 

  fBestFalseWeight = fTotalWeight + 1.0f;
  iBestStepFalseWeight = 0;
}

void CheckNoImprove() {
  if (iNoImprove) {
    if (iStep > (iBestStepNumFalse + iNoImprove)) {
      bTerminateRun = 1;
    }
  }
}


/* 
  InitVars()
  - Initialises the variables
*/

void InitVars() {
  unsigned int j;
  unsigned int k;
  unsigned int iVar = 0;
  unsigned int bAdded;
  for (j=1;j<=iNumVar;j++) {
    if (aVarInit[j] == 2) {
      aVarValue[j] = fxnRandomInt(2);
    } else {
      aVarValue[j] = aVarInit[j];
    }
  }
  if (iInitVarFlip) {
    for (j=0;j<iInitVarFlip;j++) {
      do {
        bAdded = 1;
        iVar = fxnRandomInt(iNumVar) + 1;
        if (j > 0) {
          for (k=0;k<j;k++) {
            if (aCandidateList[k] == iVar) {
              bAdded = 0;
              break;
            }
          }
        }
      } while (bAdded == 0);
      aCandidateList[j] = iVar;
      aVarValue[iVar] = 1 - aVarValue[iVar];
    }
  }
}

char sInitLine[MAXPARMLINELEN];

void InitVarFile() {
  signed int iLit;
  unsigned int iVar;
  unsigned int iValue;
  unsigned int j;
  FILE *filInit;
  char *pStart;
  char *pPos;

  for (j=1;j<=iNumVar;j++) {
    aVarInit[j] = 2;
  }

  if (strcmp(sFilenameVarInit,"")) {
    SetupFile(&filInit,"r",sFilenameVarInit,stdin,0);

    while (!feof(filInit)) {
      if (fgets(sInitLine,MAXPARMLINELEN,filInit)) {
        if (strlen(sInitLine)==MAXPARMLINELEN-1) {
          PrintUInt(pRepErr,"Unexpected Error: increase constant MAXPARMLINELEN [%d]\n",MAXPARMLINELEN);
          AbnormalExit();
        }
        if ((*sInitLine)&&(*sInitLine != '#')) {
          pStart = sInitLine;
          pPos = strchr(pStart,' ');
          while (pPos) {
            if (pPos==pStart) {
              pStart++;
            } else {
              *pPos++=0;

              sscanf(pStart,"%d",&iLit);

              if (iLit) {
                if (iLit > 0) {
                  iValue = 1;
                  iVar = iLit;
                } else {
                  iValue = 0;
                  iVar = (unsigned int) (-iLit);
                }
                aVarInit[iVar] = iValue;
              }

              pStart = pPos;
            }
            pPos = strchr(pStart,' ');
          }
          pPos = strchr(pStart,10);
          
          if (pPos) 
            *pPos = 0;
          pPos = strchr(pStart,13);
          if (pPos) 
            *pPos = 0;

          if (strlen(pStart)) {
            sscanf(pStart,"%d",&iLit);

            if (iLit) {
              if (iLit > 0) {
                iValue = 1;
                iVar = iLit;
              } else {
                iValue = 0;
                iVar = (unsigned int) (-iLit);
              }
              aVarInit[iVar] = iValue;
            }
          }
        }
      }
    }
  }
}

/* 
  CreateFalseInfo()
  - Caches a list of all the false clauses
*/

void CreateFalseInfo() {
  aFalseList = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
  aFalseListPos = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitFalseInfo() {
  unsigned int j;
  unsigned int iNum = 0;

  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0) {
      aFalseList[iNum] = j;
      aFalseListPos[j] = iNum++;      
    }
  }
}


/* 
  CreateVarLastChange()
  - Keeps track of the last time each variable was flipped
*/


void CreateVarLastChange() {
  aVarLastChange = (int*)AllocateRAM((iNumVar+1)*sizeof(int));
}

void InitVarLastChange() {
  unsigned int j;

  for (j=1;j<=iNumVar;j++) {
    aVarLastChange[j] = - (signed int) iNumVar;
  }
}

void UpdateVarLastChange() {
  aVarLastChange[iFlipCandidate] = iStep;
}


/* 
  PickRANDOM()
  - Takes a completely random walk step
*/

void PickRANDOM() {
  iFlipCandidate = fxnRandomInt(iNumVar) + 1;
}

/* 
  CheckTermination()
  - Performs a normal check to see if the solution has been found
*/

void CheckTermination() {
  if (iNumFalse <= iTarget)
    bSolutionFound = 1;
}

void CheckTerminationW() {

  unsigned int j;
  unsigned int *pClause;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;

  if (iFlipCandidate) {
  
    litWasFalse = GetTrueLit(iFlipCandidate);
    litWasTrue = GetFalseLit(iFlipCandidate);

    pClause = pLitClause[litWasTrue];
    for (j=0;j<aNumLitOcc[litWasTrue];j++) {
      if (aNumTrueLit[*pClause]==0) { 
        fFalseWeight += aClauseWeight[*pClause];
      }
      pClause++;
    }

    pClause = pLitClause[litWasFalse];
    for (j=0;j<aNumLitOcc[litWasFalse];j++) {
      if (aNumTrueLit[*pClause]==1) {
        fFalseWeight -= aClauseWeight[*pClause];
      }
      pClause++;
    }
  }

  if (fFalseWeight <= (FLOAT) iTarget)
    bSolutionFound = 1;
}



/* 
  FlipMinimal()
  - Performs a flip that maintains the minimal state info
*/

void FlipMinimal() {

  unsigned int j;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  unsigned int *pClause;

  if (iFlipCandidate == 0)
    return;

  litWasTrue = GetTrueLit(iFlipCandidate);
  litWasFalse = GetFalseLit(iFlipCandidate);

  aVarValue[iFlipCandidate] = 1 - aVarValue[iFlipCandidate];

  pClause = pLitClause[litWasTrue];
  for (j=0;j<aNumLitOcc[litWasTrue];j++)
    if (--aNumTrueLit[*pClause++]==0)
      iNumFalse++;

  pClause = pLitClause[litWasFalse];
  for (j=0;j<aNumLitOcc[litWasFalse];j++)
    if (++aNumTrueLit[*pClause++]==1)
      iNumFalse--;
}


/* 
  FlipMinimal()
  - Performs a flip that maintains the minimal state info plus the False List
*/
void FlipFalseInfo() {

  unsigned int j;
  LITTYPE litWasTrue;
  LITTYPE litWasFalse;
  unsigned int *pClause;

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
    }
    pClause++;
  }

  pClause = pLitClause[litWasFalse];
  for (j=0;j<aNumLitOcc[litWasFalse];j++) {
    aNumTrueLit[*pClause]++;
    if (aNumTrueLit[*pClause]==1) {
      aFalseList[aFalseListPos[*pClause]] = aFalseList[--iNumFalse];
      aFalseListPos[aFalseList[iNumFalse]] = aFalseListPos[*pClause];
    }
    pClause++;
  }
}

unsigned int IsLocalMinimum() {

  unsigned int j,k;
  unsigned int iNumOcc;
  unsigned int *pClause;
  LITTYPE litCur;
  signed int iScore;

  for (j=1;j<=iNumVar;j++) {

    iScore = 0;
    
    litCur = GetFalseLit(j);

    iNumOcc = aNumLitOcc[litCur];
    pClause = pLitClause[litCur];
  
    for (k=0;k<iNumOcc;k++) {
      if (aNumTrueLit[*pClause++]==0) {
        iScore--;
      }
    }

    iNumOcc = aNumLitOcc[GetNegatedLit(litCur)];
    pClause = pLitClause[GetNegatedLit(litCur)];
  
    for (k=0;k<iNumOcc;k++) {
      if (aNumTrueLit[*pClause++]==1) {
        iScore++;
      }
    }

    if (iScore < 0)
      return(0);
  }
  return(1);
}

void CreateMakeBreak() {
  aBreakCount = (unsigned int *)AllocateRAM((iNumVar+1)*sizeof(unsigned int));
  aMakeCount = (unsigned int *)AllocateRAM((iNumVar+1)*sizeof(unsigned int));
  aCritSat = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitMakeBreak() {
  unsigned int j;
  unsigned int k;
  unsigned int iVar;
  LITTYPE *pLit;
  
  memset(aMakeCount,0,(iNumVar+1)*sizeof(int));
  memset(aBreakCount,0,(iNumVar+1)*sizeof(int));
  memset(aCritSat,0,iNumClause*sizeof(unsigned int));
  
  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0) {
      for (k=0;k<aClauseLen[j];k++) {
        aMakeCount[GetVar(j,k)]++;
      }
    } else if (aNumTrueLit[j]==1) {
      pLit = pClauseLits[j];
      for (k=0;k<aClauseLen[j];k++) {
        if IsLitTrue(*pLit) {
          iVar = GetVarFromLit(*pLit);
          aBreakCount[iVar]++;
          aCritSat[j] = iVar;
          break;
        }
        pLit++;
      }
    }
  }
}

void FlipMakeBreak() {

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

      aBreakCount[iFlipCandidate]--;
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        aMakeCount[GetVarFromLit(*pLit)]++;
        pLit++;
      }
    }
    if (aNumTrueLit[*pClause]==1) {
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        if (IsLitTrue(*pLit)) {
          iVar = GetVarFromLit(*pLit);
          aBreakCount[iVar]++;
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

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCount[iVar]--;
        pLit++;
      }
      aBreakCount[iFlipCandidate]++;
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aBreakCount[aCritSat[*pClause]]--;
    }
    *pClause++;
  }
}

void CreateVarInFalseClause() {
  aVarInFalseList = (unsigned int *)AllocateRAM((iNumVar+1)* sizeof(unsigned int));
  aVarInFalseListPos = (unsigned int *)AllocateRAM((iNumVar+1)* sizeof(unsigned int));
}

void InitVarInFalseClause() {
  unsigned int j;

  iNumVarInFalseList = 0;

  for (j=1;j<=iNumVar;j++) {
    if (aMakeCount[j]) {
      aVarInFalseList[iNumVarInFalseList] = j;
      aVarInFalseListPos[j] = iNumVarInFalseList++;
    }
  }
}

void FlipMakeBreakVarInFalse() {

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

      aBreakCount[iFlipCandidate]--;
      
      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {

        iVar = GetVarFromLit(*pLit);

        aMakeCount[iVar]++;

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

      pLit = pClauseLits[*pClause];
      for (k=0;k<aClauseLen[*pClause];k++) {
        iVar = GetVarFromLit(*pLit);
        aMakeCount[iVar]--;

        if (aMakeCount[iVar]==0) {
          aVarInFalseList[aVarInFalseListPos[iVar]] = aVarInFalseList[--iNumVarInFalseList];
          aVarInFalseListPos[aVarInFalseList[iNumVarInFalseList]] = aVarInFalseListPos[iVar];
        }

        pLit++;
      }
      aBreakCount[iFlipCandidate]++;
      aCritSat[*pClause] = iFlipCandidate;
    }
    if (aNumTrueLit[*pClause]==2) {
      aBreakCount[aCritSat[*pClause]]--;
    }
    *pClause++;
  }
}

void StartSeed () {
  if (iRun==0) {
    iStartSeed = iSeed;
  } else {
    iStartSeed = fxnRandomMax();
    fxnRandomSeed(iStartSeed);
  }
}



void CreateVarScore() {
  aVarScore = (int*)AllocateRAM((iNumVar+1)*sizeof(signed int));
  aCritSat = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitVarScore() {
  unsigned int j;
  unsigned int k;
  unsigned int iVar;
  LITTYPE *pLit;
  
  memset(aVarScore,0,(iNumVar+1)*sizeof(int));
  memset(aCritSat,0,iNumClause*sizeof(unsigned int));
  
  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0) {
      for (k=0;k<aClauseLen[j];k++) {
        aVarScore[GetVar(j,k)]--;
      }
    } else if (aNumTrueLit[j]==1) {
      pLit = pClauseLits[j];
      for (k=0;k<aClauseLen[j];k++) {
        if IsLitTrue(*pLit) {
          iVar = GetVarFromLit(*pLit);
          aVarScore[iVar]++;
          aCritSat[j] = iVar;
          break;
        }
        pLit++;
      }
    }
  }
}

void FlipVarScore() {

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
    aNumTrueLit[*pClause]++;
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

VARSTATE NewVarState() {
  VARSTATE pNew = (unsigned int *)malloc(iVARSTATELen * sizeof(unsigned int));
  return(pNew);
}

void SetVarState(VARSTATE vsState) {
  unsigned int j=0;
  unsigned int k=0;
  unsigned int l=0;

  for (l=0;l<iVARSTATELen;l++) {
    vsState[l] = 0;
  }

  for (l=1;l<=iNumVar;l++) {
    if (aVarValue[l]) {
      vsState[j] |= (0x00000001 << k);
    }
    k++;
    if (k==32) {
      k=0;
      j++;
    }
  }
}

void FlipVarState(VARSTATE vsState, unsigned int iVar) {
  unsigned int j=0;

  iVar -= 1;

  while (iVar >= 32) {
    j++;
    iVar -= 32;
  }

  vsState[j] ^= (0x00000001 << iVar);

}

void PrintVarState(VARSTATE vsState) {
  unsigned int j=0;
  unsigned int k=0;
  unsigned int l=0;

  for (l=1;l<=iNumVar;l++) {
    if (vsState[j] & (0x00000001 << k)) {
      printf("1");
    }
    else {
      printf("0");
    }
    k++;
    if (k==32) {
      k=0;
      j++;
    }
  }
  printf("\n");
}

unsigned int bSameStates(VARSTATE vsA, VARSTATE vsB) {
  unsigned int j;
  for (j=0;j<iVARSTATELen;j++) {
    if (vsA[j] ^ vsB[j])
      return(0);
  }
  return(1);
}

void CopyStates(VARSTATE vsA, VARSTATE vsB) {
  unsigned int j;
  for (j=0;j<iVARSTATELen;j++) {
    vsA[j]=vsB[j];
  }
}

void AddUniqueStateToList (VARSTATEHEAD *vslHead, VARSTATE vsState) {
  
  VARSTATELISTNODE *pList;
  VARSTATELISTNODE *pLast;

  if (*vslHead) {
    pList = *vslHead;
    pLast = *vslHead;
    while (pList) {
      if (bSameStates(vsState,pList->vsState))
        return;
      pLast = pList;
      pList = pList->pNext;
    }
    pLast->pNext = (typeVARSTATELISTNODE*)malloc(sizeof(VARSTATELISTNODE));
    pLast = pLast->pNext;
  } else {
    *vslHead = (VARSTATELISTNODE*)malloc(sizeof(VARSTATELISTNODE));
    pLast = *vslHead;
  }
  pLast->vsState = NewVarState();
  CopyStates(pLast->vsState,vsState);
  pLast->pNext = 0;
}

void FreeStates(VARSTATEHEAD *vslHead) {

  VARSTATELISTNODE *pList;
  VARSTATELISTNODE *pLast;

  if (*vslHead) {
    
    pList = *vslHead;
    pLast = pList;
    pList = pList->pNext;
    while (pList) {
      free(pLast->vsState);
      free(pLast);
      pLast = pList;
      pList = pList->pNext;
    }
    free(pLast->vsState);
    free(pLast);
  }
  *vslHead = 0;
}

void GetVarState(VARSTATE vsState) {
  unsigned int j=0;
  unsigned int k=0;
  unsigned int l=0;

  for (l=1;l<=iNumVar;l++) {
    if (vsState[j] & (0x00000001 << k)) {
      aVarValue[l] = 1;
    } else {
      aVarValue[l] = 0;
    }
    k++;
    if (k==32) {
      k=0;
      j++;
    }
  }
}



void InitNullFlips() {
  iNumNullFlips = 0;
}

void UpdateNullFlips() {
  if (iFlipCandidate==0) {
    iNumNullFlips++;
  }
}

}
