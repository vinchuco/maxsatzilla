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

#include "ubcsat.h"

char sVersion[] = "0.9.8m (Beta)";
  
ALGORITHM *pAlg = 0;

int ubcsat_main( int argc, char *argv[]) {
  int i;

  // -- Reset globals
  ResetHeap();
  iNumAlg = 0;
  listFunctions.iNumItems = 0;
  iNumFound = 0;
  iRestart = 0;
  iRun = 0;
  bTerminateAllRuns = 0;
  bSolutionFound = 0;
  bTerminateRun = 0;
  iInitVarFlip = 0;
  iNumReports = 0;
  listColumns.iNumItems = 0;
  listStats.iNumItems = 0;
  fDummy = 0.0;
  iTracePenMaxLM = 0;

  parmUBCSAT.iNumParms=0;
  parmHelp.iNumParms=0;
  parmIO.iNumParms=0;
  parmAlg.iNumParms=0;

  for(i=0; i<NUMFXNTYPES; i++)
    aNumFxns[NUMFXNTYPES] = 0;

  //--------------------------
  InitSeed();
  
  BuildAlgorithmList();  
  BuildParameters();
  BuildReports();
  BuildLocal();

  CheckParamterFile(argc,argv);

  ParseParameters(&parmHelp);
  ParseParameters(&parmAlg);
  
  pAlg = GetAlgorithm();

  CheckPrintHelp();

  ParseParameters(&parmUBCSAT);
  ParseParameters(&parmIO);
  ParseParameters(&pAlg->parmList);

  CheckInvalidParamters();

  ActivateAlg(pAlg);

  SetupReports();

  fxnRandomSeed(iSeed);

  RUNFXNS(PostParameters);

  RUNFXNS(HeaderInfo);

  RUNFXNS(ReadInFile);

  RUNFXNS(CreateStructure);
  RUNFXNS(CreateStateInfo);

  iRun = 0;
  bTerminateAllRuns = 0;

  RUNFXNS(Start);

  StartClock();

  while ((iRun < iNumRuns)&&(!bTerminateAllRuns)) {

    RUNFXNS(PreRun);

    RUNFXNS(InitData);
    RUNFXNS(InitStateInfo);
    RUNFXNS(PostInit);
    
    iStep = 0;
    bSolutionFound = 0;
    bTerminateRun = 0;


    while ((iStep < iCutoff)&&(!bSolutionFound)&&(!bTerminateRun)) {
      
      iStep++;

      if (iRestart) {
        if (((iStep) % iRestart) == 0) {
          RUNFXNS(InitData);
          RUNFXNS(InitStateInfo);
        }
      }


      RUNFXNS(StartStep);
      RUNFXNS(ChooseCandidate);
      RUNFXNS(PreFlip);
      RUNFXNS(FlipCandidate);
      RUNFXNS(PostFlip);
      RUNFXNS(CheckTerminate);

      /*
       *Suppose could wrap this into CheckTerminate Function as Well
       */
      if( (fTimeout > 0) && (CurTime() > fTimeout) )
	bTerminateRun = bTerminateAllRuns = 1;

    }

    RUNFXNS(RunCalculations);
    
    RUNFXNS(PostRun);

    iRun++;

    if (bSolutionFound) {
      iNumFound++;
      if (iNumFound==iFind) {
        bTerminateAllRuns = 1;
      }
    }
  }

  StopClock();

  RUNFXNS(FinalCalculations);

  RUNFXNS(FinalReports);

  CleanExit();

  return (iNumFound > 0 ? 10 : 0);
  
}


