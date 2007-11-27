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

void ParseParameters(ALGPARMLIST *pParmList);
ALGORITHM *GetAlgorithm();

extern char sNull;

void AddParmRatio(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pNum,
                  unsigned int *pDen,
                  unsigned int iDefNum,
                  unsigned int iDefDen);

void AddParmInt(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pInt,
                  unsigned int iDefInt);

void AddParmSInt(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  signed int *pSInt,
                  signed int iDefSInt);

void AddParmBool(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pBool,
                  unsigned int bDefBool);

void AddParmFloat(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  FLOAT *pFloat,
                  FLOAT fDefFloat);

void AddParmString(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  char **pString,
                  char *sDefString);

void AddParmReport(ALGPARMLIST *pParmList,
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions
                  );

void PrintAlgSettings();

void CheckParamterFile(int iCommandLineCount,char **aCommandLineArgs);

ALGPARM *FindParm(ALGPARMLIST *pParmList, char *sSwitch);

