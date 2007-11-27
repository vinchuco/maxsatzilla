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

extern unsigned int iWpNum;
extern unsigned int iWpDen;

extern unsigned int iNumVarInFalseList;
extern unsigned int *aVarInFalseList;
extern unsigned int *aVarInFalseListPos;

void AddGWSAT();
void PickGWSAT();

void FlipMakeBreakVarInFalse();
void CreateVarInFalseClause();
void InitVarInFalseClause();

