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

void BuildReports();

extern REPORT *pRepOut;
extern REPORT *pRepErr;
extern REPORT *pRepStats;
extern REPORT *pRepRTD;
extern REPORT *pRepModel;
extern REPORT *pRepCNFStats;
extern REPORT *pRepState;
extern REPORT *pRepSolution;
extern REPORT *pRepOptClauses;
extern REPORT *pRepFlipCounts;
extern REPORT *pRepUnsatCounts;
extern REPORT *pRepVarLast;
extern REPORT *pRepClauseLast;
extern REPORT *pRepSATComp;
extern REPORT *pRepSatzilla;

}
