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

namespace ubcsat {

enum COLTYPE
{
   ColInt,
   ColFloat
};

typedef struct typeREPORT {
  FILE *fileOut;
  char *sID;
  char *sDescription;
  unsigned int bActive;
  char *sOutputFile;
  unsigned int iNumParms;
  char *aParmName[MAXREPORTPARMS];
  enum PARMTYPE aParmTypes[MAXREPORTPARMS];
  void *aParameters[MAXREPORTPARMS];
  char *sFunctions;
} REPORT;



typedef struct typeREPORTCOL {
  unsigned int bActive;

  char *sDescription;
  unsigned int iCharWidth;
  char *sHeader1;
  char *sHeader2;
  char *sHeader3;
  enum COLTYPE eType;
  char *sPrintFormat;
  void *pCurValue;

  void *pColumnData;

  char *sFunctions;

} REPORTCOL;

/*
#define CALC_MEAN       0x00000001
#define CALC_TOTAL      0x00000002
#define CALC_MEDIAN     0x00000004
#define CALC_STDDEV     0x00000008
#define CALC_VC         0x00000010

#define CALC_Q          0x00000020
#define CALC_QRATIO     0x00000040
*/



typedef struct typeREPORTSTAT {
  
  unsigned char bActive;
  unsigned int iActiveID;
  
  char *sDescription;

  unsigned int bCustomField;
  void *pCustomValue;
  enum COLTYPE eCustomType;
  char *sPrintCustomFormat;

  char *sBaseDescription;

  char *sStatParms;

  unsigned int iStatFlags;
  
  char *sRequiredCols;
  char *sDataColumn;

  char *sFunctions;

} REPORTSTAT;

extern FLOAT fDummy;

void AddColumnInt(const char *sOutputID, 
                  const char *sDescription, 
                  unsigned int iCharWidth, 
                  char *sHeader1,  
                  char *sHeader2,  
                  char *sHeader3, 
                  char *sPrintFormat, 
                  unsigned int *pCurValue,
                  char *sFunctions
                  );

void AddColumnFloat(const char *sOutputID, 
                  const char *sDescription, 
                  unsigned int iCharWidth, 
                  char *sHeader1,  
                  char *sHeader2,  
                  char *sHeader3, 
                  char *sPrintFormat, 
                  FLOAT *pCurValue,
                  char *sFunctions
                  );

void AddColumnComposite(const char *sOutputID, 
                        const char *sCompositeList);

void AddStat(const char *sStatID, 
                  const char *sDescription, 
                  const char *sBaseDescription, 
                  
                  const char *sDefParm,
                  
                  const char *sRequiredCols,
                  const char *sDataColumn,
                  const char *sFunctions
                );

void AddStatComposite(const char *sStatID, 
                      const char *sCompositeList);

void AddStatCustom(const char *sStatID, 
                  const char *sDescription, 
                  const char *sPrintCustomFormat,
                  void *pCurValue,
                  enum COLTYPE eCustomType,
                  const char *sRequiredCols,
                  const char *sFunctions
                );

#define NUMVALIDSTATCODES 17
extern char *sValidStatCodes[];


void CreateReports();

void PrintColHeaders();
void PrintRow();
void UpdateOutputRow();

void PrintStats();
void SatzillaPrintStats();
void PrintRTD();

void SortByStepPerformance(); 
void UpdatePercents();
void UpdateTimes();

void PrintTraceSolution();

void CreateFlipCounts();
void InitFlipCounts();
void UpdateFlipCounts();
void PrintTraceFlipCounts();

void CreateUnsatCounts();
void InitUnsatCounts();
void UpdateUnsatCounts();
void PrintTraceUnsatCounts();

void PrintTraceVarLast();

void CreateClauseLast();
void InitClauseLast();
void UpdateClauseLast();
void PrintTraceClauseLast();


void InitSolveMode();
void PrintSolutionModel();
void PrintSATCompetition();
void PrintCNFStats();

void CalcPercentSolve();
void CalcFPS();

extern FLOAT fPercentSuccess;
extern FLOAT fFlipsPerSecond;

extern FLOAT fPercentNullFlips;

void PrintTraceState();

REPORT *AddReport(const char *sID, const char *sDescription, const char *sOutputFile, const char *sFunctions);
void AddReportParm(REPORT *pRep, const char *sParmName, enum PARMTYPE eType, void *pDefault);

extern unsigned int iNumReports;
extern REPORT aReports[MAXREPORTS];

void ActivateColumnName(char *sIn);
void ActivateFunction(unsigned int iFxnID, const char *sItem);

extern ITEMLIST listColumns;
extern ITEMLIST listStats;

extern REPORTCOL aColumns[];
extern REPORTSTAT aStats[];
void ParseItemList(ITEMLIST *pList, char *sItems, CALLBACKPTR ItemFunction);

void SetupOutput();
void ActivateColumn(unsigned int iColID, const char *sItem);

void UpdateColumnsRow();

void PrintHeadersOut();
void PrintHeadersRTD();
void PrintCurRow();

void ActivateStat(unsigned int iStatID, const char *sItem);

void SetupStats();

void SetupRTD();

void PrintTraceOptClauses();

extern unsigned int *aFlipCount;
extern unsigned int *aUnsatCount;
extern signed int *aClauseLast;

void CalcPercentNullFlips();

extern char *sCommentString;


/**** 0.9.8m changes */

extern unsigned int iStartFalse;
extern FLOAT fImproveMean;

void SetStartFalse();
void CalcImproveMean();

extern unsigned int iFirstLMFalse;
extern unsigned int iNumBestFalseTrack;
extern FLOAT  fFirstLMRatio;

void CreateBestTrack();
void InitBestTrack();
void UpdateBestTrack();
void FinalBestTrack();

extern FLOAT fBestFalseTrackMean;
extern FLOAT fBestFalseTrackCV;


/* 0.9.8m changes ****/


}
