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

namespace ubcsat {

/* 
  BuildReports()
  - Central routine for adding reports to ubcsat
*/

REPORT repDefErr;

REPORT *pRepOut;
REPORT *pRepErr = &repDefErr;
REPORT *pRepStats;
REPORT *pRepRTD;
REPORT *pRepState;
REPORT *pRepModel;
REPORT *pRepSolution;
REPORT *pRepOptClauses;
REPORT *pRepCNFStats;
REPORT *pRepFlipCounts;
REPORT *pRepUnsatCounts;
REPORT *pRepVarLast;
REPORT *pRepClauseLast;
REPORT *pRepSATComp;
REPORT *pRepPenalty;
REPORT *pRepPenSol;
REPORT *pRepSatzilla;

void PrintPenaltyTrace() {
  unsigned int j;
  if (bClausePenaltyCreated) {
    if (iFlipCandidate)
      return;
    if ((iNumNullFlips<iTracePenMaxLM)||(iTracePenMaxLM==0)) {
      if ((iRun==0)&&(iNumNullFlips==1)) {
        PrintHdr(pRepPenSol,1,"Run ID, Local Min #, Penalty[0], Penalty[1]...\n");
      }
      PrintUInt(pRepPenSol,"%d,",iRun);
      PrintUInt(pRepPenalty,"%d",iNumNullFlips);
      for (j=0;j<iNumClause;j++) {
        PrintFloat(pRepPenalty,",%f",aClausePenalty[j]);
      }
      Print(pRepPenalty,"\n");
    }
  }
}

void PrintPenaltySolutionTrace() {
  unsigned int j;

  if (bClausePenaltyCreated) {
    if (!bSolutionFound)
      return;
    if (iRun==0) {
      PrintHdr(pRepPenSol,1,"Run ID, Penalty[0], Penalty[1]...\n");
    }
    PrintUInt(pRepPenSol,"%d",iRun);
    for (j=0;j<iNumClause;j++) {
      PrintFloat(pRepPenSol,",%f ",aClausePenalty[j]);
    }
    Print(pRepPenSol,"\n");
  }
}

void BuildReports() {

  pRepOut = AddReport("out", "Standard Output -- Shows the results (columns) from each run (row)","null", "SetupOutput,PrintSplash,PrintAlgSettings,PrintHeadersOut,PrintCurRow");

  pRepOut->bActive = 1;
  AddReportParm(pRepOut,"Columns to Display (ubcsat -hc for info)",TypeString,(void *) "default");

  pRepStats = AddReport("stats","Statistics Report -- Shows summary statistics from all runs","stdout","PrintStats");
  /*  pRepStats->bActive = 1;*/
  AddReportParm(pRepStats,"Statistics to Display (ubcsat -hs for info)",TypeString,(void *) "default");

  pRepSatzilla = AddReport("satzilla","Like stats, but do it for satzilla","stdout","SatzillaPrintStats");
  pRepSatzilla->bActive = 1;
  //  AddReportParm(pRepSatzilla,"Statistics to Display (ubcsat -hs for info)",TypeString,(void *) "default");
  AddReportParm(pRepSatzilla,"Statistics to Display (ubcsat -hs for info)",TypeString,(void *) "best[mean;cv],beststep[mean;cv;median;q10;q90],avgimpr[mean;cv],firstlmratio[mean;cv],bestcv[mean],totaltime");

    
  pRepErr = AddReport("err","Error Report -- All errors that are encountered (defaults to stderr)","stderr","");
  pRepErr->bActive = 1;
  pRepErr->fileOut = stderr;

  pRepRTD = AddReport("rtd","Run-Time Distribution -- Results from each run, sorted by step performance", "stdout","PrintRTD");
  AddReportParm(pRepRTD,"Columns to Display (ubcsat -hc for more info)",TypeString,(void *) "rtd");

  pRepModel = AddReport("model","Solution Model -- Ouput of -solve: show the solution model of the last run","stdout","PrintSolutionModel");

  pRepCNFStats = AddReport("cnfstats","CNF Stats -- Detailed information about the instance","stdout","PrintCNFStats");

  pRepState = AddReport("state","State Data -- Detailed state information for each search step","stdout","TraceState");

  pRepSolution = AddReport("solution","Solutions -- Solution for every successful run","stdout","TraceSolution");
  
  pRepOptClauses = AddReport("unsatclauses","Print unsatisfied clauses when (nonzero) -target is reached","stdout","TraceOptClauses");

  pRepFlipCounts = AddReport("flipcount","Flip Count -- Number of times each var was flipped","stdout","TraceFlipCounts");

  pRepUnsatCounts = AddReport("unsatcount","UnSat Count -- Number of times each clause was unsat","stdout","TraceUnsatCounts");

  pRepVarLast = AddReport("varlastflip","Variable Last Flip -- Step of most recent flip","stdout","TraceVarLast");

  pRepClauseLast = AddReport("clauselast","Clause Last UnSat -- Last step each clause was unsatisfied","stdout","TraceClauseLast");

  pRepPenSol = AddReport("pensol","Final DLS Penalties at solution","stdout","TracePenaltySolution");
  pRepPenalty = AddReport("penalty","Penalty Trace -- DLS penalties at every local minimum","stdout","TracePenalty");
  AddReportParm(pRepPenalty,"Max # LM to trace",TypeInt,(void *) &iTracePenMaxLM);

  /* NOTE: SAT Competition should be the last report, as it calls exit() */
  pRepSATComp = AddReport("satcomp","SAT Competition Output (use -solve)","stdout","PrintSATCompetition");


  AddFunction("SetupOutput",CreateStructure,SetupOutput,"","");
  
  AddFunction("PrintSplash",HeaderInfo,PrintSplash,"","");

  AddFunction("PrintAlgSettings",Start,PrintAlgSettings,"","");
  AddFunction("PrintHeadersOut",Start,PrintHeadersOut,"","");

  AddFunction("SetupStats",CreateStructure,SetupStats,"UpdateColumnsRow","");
  AddFunction("PrintStats",FinalReports,PrintStats,"SetupStats,SortByStepPerformance","");
  AddFunction("SatzillaPrintStats",FinalReports,SatzillaPrintStats,"SetupStats,SortByStepPerformance","");

  AddFunction("SetupRTD",CreateStructure,SetupRTD,"UpdateColumnsRow","");
  AddFunction("PrintHeadersRTD",FinalReports,PrintHeadersRTD,"","");
  AddFunction("PrintRTD",FinalReports,PrintRTD,"SetupRTD,PrintHeadersRTD","");


  AddFunction("UpdateColumnsRow",PostRun,UpdateColumnsRow,"","");
  AddFunction("PrintCurRow",PostRun,PrintCurRow,"UpdateColumnsRow","");

  AddFunction("InitBestFalse",PreRun,InitBestFalse,"","");
  AddFunction("BestFalse",PostFlip,CheckBestFalse,"InitBestFalse","");

  AddFunction("SortByStepPerformance",FinalCalculations,SortByStepPerformance,"","");
  AddFunction("UpdatePercents",FinalCalculations,UpdatePercents,"SortByStepPerformance","");
  AddFunction("UpdateTimes",FinalCalculations,UpdateTimes,"SortByStepPerformance","");
  
  AddFunction("CalcPercentSolve",FinalCalculations,CalcPercentSolve,"","");
  AddFunction("CalcFPS",FinalCalculations,CalcFPS,"","");
  
  AddFunction("TraceSolution",PostRun,PrintTraceSolution,"","");

  AddFunction("CreateFlipCounts",CreateStateInfo,CreateFlipCounts,"","");
  AddFunction("InitFlipCounts",InitStateInfo,InitFlipCounts,"CreateFlipCounts","");
  AddFunction("FlipCounts",PostFlip,UpdateFlipCounts,"InitFlipCounts,CreateFlipCounts","");
  AddFunction("TraceFlipCounts",PostRun,PrintTraceFlipCounts,"FlipCounts","");

  AddFunction("CreateUnsatCounts",CreateStateInfo,CreateUnsatCounts,"","");
  AddFunction("InitUnsatCounts",InitStateInfo,InitUnsatCounts,"CreateUnsatCounts","");
  AddFunction("UnsatCounts",PostFlip,UpdateUnsatCounts,"InitUnsatCounts,CreateUnsatCounts","");
  AddFunction("TraceUnsatCounts",PostRun,PrintTraceUnsatCounts,"UnsatCounts","");

  AddFunction("TraceVarLast",PostRun,PrintTraceVarLast,"VarLastChange","");

  AddFunction("CreateClauseLast",CreateStateInfo,CreateClauseLast,"","");
  AddFunction("InitClauseLast",InitStateInfo,InitClauseLast,"CreateClauseLast","");
  AddFunction("ClauseLast",PostFlip,UpdateClauseLast,"InitClauseLast,CreateClauseLast","");
  AddFunction("TraceClauseLast",PostRun,PrintTraceClauseLast,"ClauseLast","");

  AddFunction("TraceState",PostFlip,PrintTraceState,"","");

  AddFunction("SolveMode",Start,InitSolveMode,"PrintSATCompetition","");
  /*  AddFunction("SolveMode",Start,InitSolveMode,"PrintSolutionModel","");*/
  AddFunction("PrintSolutionModel",FinalReports,PrintSolutionModel,"","");
  AddFunction("PrintSATCompetition",FinalReports,PrintSATCompetition,"","");

  AddFunction("NoImprove",CheckTerminate,CheckNoImprove,"BestFalse","");

  AddFunction("PrintCNFStats",FinalReports,PrintCNFStats,"","");

  AddFunction("TraceOptClauses",PostRun,PrintTraceOptClauses,"","");

  AddFunction("StartSeed",PreRun,StartSeed,"","");

  AddFunction("PercentNullFlips",RunCalculations,CalcPercentNullFlips,"NullFlips","");

  AddFunction("SetupCountRandom",Start,SetupCountRandom,"","");
  AddFunction("InitCountRandom",PreRun,InitCountRandom,"","");
  CreateCompoundFunction("CountRandom","SetupCountRandom,InitCountRandom");

  AddFunction("TracePenalty",PostFlip,PrintPenaltyTrace,"NullFlips","");
  AddFunction("TracePenaltySolution",PostRun,PrintPenaltySolutionTrace,"","");

  AddColumnComposite("default","run,found,best,beststep,steps");
  AddColumnComposite("rtd","prob,steps,time");

  AddColumnInt("run","Run Number",5,"     ","  Run","  No.","%5u",&iRun,"");
  AddColumnInt("found","Solution Found (1 => yes)",1,"F","N","D","%1u",&bSolutionFound,"");
  AddColumnInt("steps","Total Number of Search Steps",10,"     Total","    Search","     Steps","%10u",&iStep,"");

  AddColumnInt("best","Best Solution Found",5," Best","Sol'n","Found","%5u",&iBestNumFalse,"BestFalse");
  AddColumnInt("beststep","Step of Best Solution Found",10,"      Step","        of","      Best","%10u",&iBestStepNumFalse,"BestFalse");
  
  AddColumnInt("seed","Starting Seed For the Run",10,"          ","  Starting","      Seed","%10u",&iStartSeed,"StartSeed");

  AddColumnInt("nullflips","Number of Null Flips",10,"    Number","   of Null","     Flips","%10u",&iNumNullFlips,"NullFlips");
  AddColumnInt("rand","Number of Random Decisions",10,"    Number"," of Random"," Decisions","%10u",&iNumRandomCalls,"CountRandom");
  AddColumnFloat("percentnull","Percent of Null Flips",7,"Percent","   Null","  Flips","%7.4f",&fPercentNullFlips,"NullFlips,PercentNullFlips");

  AddColumnFloat("prob","Probability of Success (RTD Only)",7,"   Prob","     of","Success","%7.5f",&fDummy,"UpdatePercents");
  AddColumnFloat("time","Time in seconds (RTD Only)",10,"  CPU Time","        in","   seconds","%10.6f",&fDummy,"UpdateTimes");

  AddStatComposite("default","totaltime,fps,percentsolve,stepsall,time,instinfo");
  AddStatCustom("numclause","Number of Clauses","Clauses = %d\n",&iNumClause,ColInt,"","");
  AddStatCustom("numvars","Number of Variables","Variables = %d\n",&iNumVar,ColInt,"","");
  AddStatCustom("numlits","Number of Total Literals","TotalLiterals = %d\n",&iNumLit,ColInt,"","");
  AddStatCustom("percentsolve","Percent of Successful Runs","PercentSuccess = %f\n",&fPercentSuccess,ColFloat,"","CalcPercentSolve");
  AddStatCustom("totaltime","Total Time Elapsed","TotalCPUTimeElapsed = %f\n",&fTotalTime,ColFloat,"","");

  AddStatCustom("fps","Flips Per Second","FlipsPerSecond = %f\n",&fFlipsPerSecond,ColFloat,"","CalcFPS");
  AddStat("steps","Search Steps ","Steps","mean;median","steps","steps","");
  AddStat("stepsall","All Search Step Statistics","Steps","all","steps","steps","");
  AddStat("time","Time (in CPU seconds)","CPUTime","mean;median","time","time","");
  AddStat("nullflips","Null Flips","NullFlips","mean;median","nullflips","nullflips","");
  AddStat("percentnull","Percent Null Flips","PercentNull","mean;median","percentnull","percentnull","");

  AddStat("best","Best Solution Found","BestSolution","mean;median","best","best","");
  AddStat("beststep","Step of Best Solution Found","BestStep","mean;median","beststep","beststep","");


  AddStatComposite("instinfo","numvars,numclause,numlits");


  /**** 0.9.8m changes */

  AddColumnInt("startfalse","Number of False Clauses at Initialisation",6," # of ","false ","@start","%6u",&iStartFalse,"StartFalse");
  AddFunction("StartFalse",PostInit,SetStartFalse,"","");

  AddColumnInt("endfalse","Number of False Clauses at Termination",6," # of ","false "," @end ","%6u",&iNumFalse,"");

  AddColumnFloat("avgimpr","Mean Improvement per Step to Best Solution",7,"Avg Imp","/ step ","to best","%7.5f",&fImproveMean,"CalcImproveMean");
  AddFunction("CalcImproveMean",RunCalculations,CalcImproveMean,"StartFalse,BestFalse","");

  AddColumnInt("firstlm","Number of False Clauses at First Local Minimum",6," # of ","false ","@1stlm","%6u",&iFirstLMFalse,"BestTrack");

  AddFunction("CreateBestTrack",CreateStateInfo,CreateBestTrack,"","");
  AddFunction("InitBestTrack",InitStateInfo,InitBestTrack,"CreateBestTrack","");
  AddFunction("UpdateBestTrack",PostFlip,UpdateBestTrack,"BestFalse,CreateBestTrack,InitBestTrack","");
  AddFunction("FinalBestTrack",RunCalculations,FinalBestTrack,"UpdateBestTrack,BestFalse,CreateBestTrack,InitBestTrack","");
  CreateCompoundFunction("BestTrack","FinalBestTrack,UpdateBestTrack,BestFalse,CreateBestTrack,InitBestTrack");

  AddColumnInt("bestcount","Number of New 'Best Solutions' Encountered",4,"# of","Best","Soln","%4u",&iNumBestFalseTrack,"BestTrack");
  AddColumnFloat("bestmean","Mean of # False in New 'Best Solutions'",7,"Avg. of","# false","in best","%7.3f",&fBestFalseTrackMean,"BestTrack");
  AddColumnFloat("bestcv","Coefficient of Variation of # False in New 'Best Solutions'",7,"c.v. of","# false","in best","%7.3f",&fBestFalseTrackCV,"BestTrack");


  AddColumnInt("startfalse","Number of False Clauses at Initialisation",6," # of ","false ","@start","%6u",&iStartFalse,"StartFalse");
  AddColumnFloat("avgimpr","Mean Improvement per Step to Best Solution",7,"Avg Imp","/ step ","to best","%7.5f",&fImproveMean,"CalcImproveMean");
  AddColumnInt("firstlm","Number of False Clauses at First Local Minimum",6," # of ","false ","@1stlm","%6u",&iFirstLMFalse,"BestTrack");
  AddColumnFloat("firstlmratio","Number of False Clauses at First Local Minimum over total improvement",7," 1stlm ","over ","tot.imp","%7.5f",&fFirstLMRatio,"BestTrack");

  AddStat("startfalse","Number of False Clauses at Initialisation","StartFalse","mean;median","startfalse","startfalse","");
  AddStat("avgimpr","Mean Improvement per Step to Best Solution","AvgImproveToBest","mean;median","avgimpr","avgimpr","");
  AddStat("firstlm","Number of False Clauses at First Local Minimum","FirstLM","mean;median","firstlm","firstlm","");

  AddStat("firstlmratio","Fraction of improvement due to first local minimum","FirstLMRatio","mean;median","firstlmratio","firstlmratio","");

  AddStat("bestcount","Number of New 'Best Solutions' Encountered","BestCount","mean;median","bestcount","bestcount","");
  AddStat("bestmean","Mean of # False in New 'Best Solutions'","BestMean","mean;median","bestmean","bestmean","");
  AddStat("bestcv","Coefficient of Variation of # False in New 'Best Solutions'","BestCV","mean;median","bestcv","bestcv","");

  /* 0.9.8m changes ****/

}

}
