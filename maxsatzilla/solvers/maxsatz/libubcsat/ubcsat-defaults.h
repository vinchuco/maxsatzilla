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


#define NOFXN 0
#define NOREF(A) (A)=(A)

extern ALGORITHM *pDefaultAlg;

extern ALGPARMLIST parmUBCSAT;
extern ALGPARMLIST parmHelp;
extern ALGPARMLIST parmIO;
extern ALGPARMLIST parmAlg;

extern char sNull;

extern char *sAlgName;
extern char *sVarName;
extern unsigned int bWeighted;

extern unsigned int bShowHelp;
extern unsigned int bShowHelpA;
extern unsigned int bShowHelpV;
extern unsigned int bShowHelpT;
extern unsigned int bShowHelpR;
extern unsigned int bShowHelpC;
extern unsigned int bShowHelpS;

extern unsigned int bReportEcho;
extern unsigned int bReportClean;

extern unsigned int iNumRuns;
extern unsigned int iCutoff;
extern unsigned int iSeed;
extern unsigned int iTarget;
extern unsigned int iFind;
extern unsigned int iNumFound;
extern unsigned int iRestart;

extern unsigned int iRun;
extern unsigned int iStep;

extern char *sFilenameIn;
extern char *sFilenameParms;
extern char *sFilenameVarInit;

extern unsigned int *aBreakCount;
extern unsigned int *aMakeCount;
extern unsigned int *aCritSat;

extern signed int *aVarScore;

extern unsigned int *aNumLitOcc;
extern unsigned int *aLitOccData;
extern unsigned int **pLitClause;

extern unsigned int iNumFalse;
extern unsigned int *aFalseList;
extern unsigned int *aFalseListPos;

extern signed int *aVarLastChange;

extern FLOAT fFalseWeight;
extern FLOAT fBestFalseWeight;

extern unsigned int iBestNumFalse;
extern unsigned int iBestStepNumFalse;

extern unsigned int *aNumTrueLit;
extern unsigned int *aVarValue;

extern unsigned int iFlipCandidate;
extern unsigned int *aCandidateList;
extern unsigned int iNumCandidates;
extern int iBestScore;

extern unsigned int iNoImprove;
extern FLOAT fNoImproveFactor;

extern FLOAT fTimeout;

extern unsigned int bTerminateAllRuns;
extern unsigned int bSolutionFound;
extern unsigned int bTerminateRun;
extern unsigned int bSolveMode;
extern unsigned int bSatzillaMode;

extern unsigned int iStartSeed;

extern unsigned int iInitVarFlip;

void AddDEFAULT();

void CheckTermination();

void CheckTerminationW();
void InitMinimalStateInfoW();

void CreateLitOccurence();
void CreateMinimalStateInfo();
void InitMinimalStateInfo();
void CreateCandidateInfo();
void InitCandidateInfo();
void CreateFalseInfo();
void InitFalseInfo();

void InitVarLastChange();
void CreateVarLastChange();
void UpdateVarLastChange();

void FlipMinimal();
void FlipFalseInfo();

void PickRANDOM();

void BuildParameters();

void InitBestFalse();
void CheckBestFalse();

void CheckNoImprove();

unsigned int IsLocalMinimum();

void InitVars();

void StartSeed ();

void CreateVarScore();
void InitVarScore();
void FlipVarScore();

typedef unsigned int *VARSTATE;

VARSTATE NewVarState();

typedef struct typeVARSTATELISTNODE {
  VARSTATE vsState;
  struct typeVARSTATELISTNODE *pNext;
} VARSTATELISTNODE;

typedef VARSTATELISTNODE *VARSTATEHEAD;


void SetVarState(VARSTATE vsState);
void PrintVarState(VARSTATE vsState);
void CopyStates(VARSTATE vsA, VARSTATE vsB);
void AddUniqueStateToList (VARSTATEHEAD *vslHead, VARSTATE vsState);
void FreeStates(VARSTATEHEAD *vslHead);
void GetVarState(VARSTATE vsState);
void FlipVarState(VARSTATE vsState, unsigned int iVar);
unsigned int bSameStates(VARSTATE vsA, VARSTATE vsB);


