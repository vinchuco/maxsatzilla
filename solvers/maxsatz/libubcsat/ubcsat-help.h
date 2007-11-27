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

void HelpNoAlgorithm();
void HelpBadParm(char *sParm);
void HelpBadReport(char *sParm);


void HelpPrintAlgorithms();
void HelpShow();
void HelpShowV();
void HelpShowT();
void HelpPrintReports();
void HelpPrintColumns();
void HelpPrintStats();
void CheckPrintHelp();
void CheckInvalidParamters();

void PrintSplash();
void PrintUBCSATHeader(REPORT *pRep);
void PrintAlgParmSettings(REPORT *pRep, ALGPARMLIST *pParmList);

