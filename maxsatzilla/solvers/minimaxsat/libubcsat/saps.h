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

extern FLOAT fAlpha;
extern FLOAT fRho;
extern unsigned int iPsNum;
extern unsigned int iPsDen;
extern unsigned int iWpNum;
extern unsigned int iWpDen;

extern FLOAT *aClausePenalty;
extern FLOAT *aMakePenalty;
extern FLOAT *aBreakPenalty;
extern FLOAT fPenaltySum;
extern FLOAT fPenaltyImprove;

extern unsigned int iTracePenMaxLM;

extern unsigned int iNumNullFlips;

extern unsigned int bClausePenaltyCreated;

void AddSAPS();
void CreateClausePenalty();
void InitClausePenalty();

void PickSAPS();
void FlipSAPS();
void PostFlipSAPS();

void InitNullFlips();
void UpdateNullFlips();
void PrintPenaltyTrace();
void PrintPenaltySolutionTrace();

void InitMakeBreakPenalty();
void InitClausePenaltyRandom();

void SmoothSAPS();
void AdjustPenalties();
void ScaleSAPS();

void PickSAPSNoRandom();
void PostFlipSAPSNoRandom();

}
