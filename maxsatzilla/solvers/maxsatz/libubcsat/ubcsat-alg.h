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

enum PARMTYPE {
   TypeInt,
   TypeSInt,
   TypeBool,
   TypeString,
   TypeRatio,
   TypeFloat,
   TypeReport
};

enum FXNTYPE
{
  PostParameters,
  HeaderInfo,
  ReadInFile,
  CreateStructure,
  CreateStateInfo,
  Start,
  PreRun,
  InitData,
  InitStateInfo,
  PostInit,
  StartStep,
  ChooseCandidate,
  PreFlip,
  FlipCandidate,
  PostFlip,
  CheckTerminate,
  RunCalculations,
  PostRun,
  FinalCalculations,
  FinalReports,
  NUMFXNTYPES
};

typedef void (*FXNPTR)();
typedef void (*CALLBACKPTR)(unsigned int, const char *sItem); 

typedef struct typeITEM {
  char *sID;
  unsigned int bCompound;
  char *sCompoundList;
} ITEM;

typedef struct typeITEMLIST {
  unsigned int iNumItems;
  ITEM aItems[MAXITEMLIST];
} ITEMLIST;

void AddItem(ITEMLIST *pList,const char *sID);
void AddCompoundItem(ITEMLIST *pList,const char *sID, const char *sList);

typedef struct typeFXN {
  enum FXNTYPE eFType;
  FXNPTR fxnFunction;
  char *sDependencies;
  char *sReplace;
} FXN;

typedef union typePARAMETERDEFAULT {
  unsigned int iInt;
  signed int iSInt;
  unsigned int bBool;
  char *sString;
  FLOAT fFloat;
} PARAMETERDEFAULT;

typedef struct typeALGPARM {
  enum PARMTYPE eType;
  char *sSwitch;
  char *sName;
  char *sDescription;
  void *pParm1;
  void *pParm2;
  unsigned int bSpecified;
  char *sFunctions;
  PARAMETERDEFAULT defDefault1;
  PARAMETERDEFAULT defDefault2;
} ALGPARM;


typedef struct ALGPARAMETERS {
  unsigned int iNumParms;
  ALGPARM aParms[MAXALGPARMS];
} ALGPARMLIST;

typedef struct typeALGORITHM {
  char *sName;
  char *sVariant;
  unsigned int bWeighted;
  char *sDescription;
  char *sFunctions;
  ALGPARMLIST parmList;
} ALGORITHM;

#define RUNFXNS(A) {for (iFunctionLoop=0;iFunctionLoop<aNumFxns[A];iFunctionLoop++) aFxns[A][iFunctionLoop](); }
#define RUNFXNS2(A) {for (iFunctionLoop2=0;iFunctionLoop2<aNumFxns[A];iFunctionLoop2++) aFxns[A][iFunctionLoop2](); }

extern ALGORITHM aAlgorithms[];
extern unsigned int iNumAlg;

extern ALGORITHM *pAlg;
extern char sVersion[];

extern FXN aFunctions[];
extern unsigned int iNumFunctions;

extern FXNPTR aFxns[][MAXFXNLIST];
extern unsigned int aNumFxns[];
extern unsigned int iFunctionLoop;
extern unsigned int iFunctionLoop2;

void BuildAlgorithmList();
ALGORITHM *AddAlg(const char *sName, const char *sVariant, unsigned int bWeighted, 
                  const char *sDescription, 
                  const char *sFunctions);


void CopyParameters(ALGORITHM *pDest, const char *sName, const char *sVar, unsigned int bWeighted);

void AddFunction(const char *sID, enum FXNTYPE eFType, FXNPTR fxnFunction, char *sDependencies, char *sReplace);

extern ITEMLIST listFunctions;

void CreateCompoundFunction(const char *sID, const char *sList);

void ActivateAlg(ALGORITHM *pAlgActivate);

