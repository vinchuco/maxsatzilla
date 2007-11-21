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

extern unsigned int iNumVar;
extern unsigned int iVARSTATELen;
extern unsigned int iNumClause;
extern unsigned int iNumLit;
extern unsigned int *aClauseLen;
extern FLOAT *aClauseWeight;
extern FLOAT fTotalWeight;
extern LITTYPE **pClauseLits;

void ReadCNF();
void CreateLitOccurence();

}
