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

typedef void (*FXNRANDSEED)(unsigned int);
typedef unsigned int (*FXNRANDMAX)();
typedef unsigned int (*FXNRANDINT)(unsigned int);
typedef unsigned int (*FXNRANDRATIO)(unsigned int, unsigned int);
typedef FLOAT (*FXNRANDFLOAT)();

extern FXNRANDSEED fxnRandomSeed;
extern FXNRANDMAX fxnRandomMax;
extern FXNRANDINT fxnRandomInt;
extern FXNRANDRATIO fxnRandomRatio;
extern FXNRANDFLOAT fxnRandomFloat;

void Print(REPORT *pRep, const char *sFormat);
void PrintUInt(REPORT *pRep, const char *sFormat, unsigned int iVal);
void PrintSInt(REPORT *pRep, const char *sFormat, signed int iVal);
void PrintFloat(REPORT *pRep, const char *sFormat, FLOAT fVal);
void PrintString(REPORT *pRep, const char *sFormat, char *sString);

void PrintHdr(REPORT *pRep, unsigned int bComment, const char *sFormat);
void PrintHdrUInt(REPORT *pRep, unsigned int bComment, const char *sFormat, unsigned int iVal);
void PrintHdrSInt(REPORT *pRep, unsigned int bComment, const char *sFormat, signed int iVal);
void PrintHdrFloat(REPORT *pRep, unsigned int bComment, const char *sFormat, FLOAT fVal);
void PrintHdrString(REPORT *pRep, unsigned int bComment, const char *sFormat, char *sString);

void SetupReports();
void AbnormalExit();
void CleanExit();

void CloseSingleFile(FILE *filToClose);

void SetupFile(FILE **fFil,const char *sOpenType, const char *sFilename, FILE *filDefault, unsigned int bAllowNull);

extern unsigned int iNumRandomCalls;
void SetupCountRandom();
void InitCountRandom();

void InitVarFile();

