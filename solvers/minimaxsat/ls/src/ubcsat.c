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
  ... project website: http://www.satlib.org/ubcsat ....
  ------------------------------------------------------
  .....e-mail ubcsat-help [@] cs.ubc.ca for support.....
  ------------------------------------------------------

*/

#include <stdio.h>
#include "ubcsat.h"


char sVersion[] = "1.0.0 (Grouse Mountain Release)";

void saveResult() {
	int j;
	FILE *ft;
	char line[1000];
	
	sprintf(line,"ls_id_%d.txt",iProcId);
	ft = fopen(line, "w");
	fprintf(ft,"%d %f\n",iBestNumFalse,fBestSumFalseW);
	
	for (j=1;j<=iNumVars;j++) {
		if (!aVarStateBest[j]) fprintf(ft,"-%u\n",j);
		else fprintf(ft,"%u\n",j);
	}
	fclose(ft);
}

int ubcsatmain(int argc, char *argv[]) {
  
  InitSeed();

  SetupUBCSAT();

  AddAlgorithms();
  AddParameters();
  AddReports();
  AddDataTriggers();
  AddReportTriggers();

  AddLocal();
  
  ParseAllParameters(argc,argv);

  ActivateAlgorithmTriggers();
  ActivateReportTriggers();

  RandomSeed(iSeed);

  RunProcedures(PostParameters);

  RunProcedures(ReadInInstance);

  RunProcedures(CreateData);
  RunProcedures(CreateStateInfo);

  iRun = 0;
  bTerminateAllRuns = FALSE;

  RunProcedures(PreStart);

  StartClock();

  while ((iRun < iNumRuns) && (! bTerminateAllRuns)) {

    iRun++;

    iStep = 0;
    bSolutionFound = FALSE;
    bTerminateRun = FALSE;
    bRestart = TRUE;

    RunProcedures(PreRun);
    
    while ((iStep < iCutoff) && (! bSolutionFound) && (! bTerminateRun)) {

      iStep++;

      RunProcedures(PreStep);
      RunProcedures(CheckRestart);

      if (bRestart) {
        RunProcedures(InitData);
        RunProcedures(InitStateInfo);
        RunProcedures(PostInit);
        bRestart = FALSE;
      } else {
        RunProcedures(ChooseCandidate);
        RunProcedures(PreFlip);
        RunProcedures(FlipCandidate);
        RunProcedures(UpdateStateInfo);
        RunProcedures(PostFlip);
      }
      
      RunProcedures(PostStep);

      RunProcedures(StepCalculations);

      RunProcedures(CheckTerminate);
    }

    RunProcedures(RunCalculations);
    
    RunProcedures(PostRun);

    if (bSolutionFound) {
      iNumSolutionsFound++;
      if (iNumSolutionsFound == iFind) {
        bTerminateAllRuns = TRUE;
      }
    }
  }

  StopClock();

  
  RunProcedures(FinalCalculations);

  RunProcedures(FinalReports);
  
  saveResult();

  CleanExit();

  return(0);
  
}

#ifndef ALTERNATEMAIN

int main(int argc, char *argv[]) {
  return(ubcsatmain(argc,argv));
}

#endif
