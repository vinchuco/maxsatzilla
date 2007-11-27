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

void CheckPrintHelp() {
  if (bShowHelpA) HelpPrintAlgorithms();
  if (bShowHelpV) HelpShowV();
  if (bShowHelpT) HelpShowT();
  if (bShowHelpR) HelpPrintReports();
  if (bShowHelpC) HelpPrintColumns();
  if (bShowHelpS) HelpPrintStats();
  if (bShowHelp)  HelpShow();
  if (!pAlg) HelpNoAlgorithm();
}

void PrintUBCSATHeader(REPORT *pRep) {
  PrintHdr(pRep,1,"\n");
  PrintHdrString(pRep,1,"UBCSAT version %s\n",sVersion);
  PrintHdr(pRep,1,"\n");
  PrintHdr(pRep,1,"\n");
  PrintHdr(pRep,1,"ubcsat -h for help\n");
  PrintHdr(pRep,1,"\n");
}

void PrintSplash() {
  PrintUBCSATHeader(pRepOut);
  if (strcmp(sFilenameIn,"")==0) {
    PrintHdr(pRepOut,1,"no -inst file specified: reading from stdin -- e.g.: ubcsat <myfile.cnf\n");
    PrintHdr(pRepOut,1,"\n");
  }
}

void HelpPrintAlgorithms() {
  unsigned int j;

  PrintUBCSATHeader(pRepOut);
  Print(pRepOut,"\nSupported algorithms:\n\n");

  for (j=0;j<iNumAlg;j++) {
    PrintString(pRepOut," -alg %s",aAlgorithms[j].sName);
    if (*aAlgorithms[j].sVariant) {
      PrintString(pRepOut," -v %s",aAlgorithms[j].sVariant);
    }
    if (aAlgorithms[j].bWeighted) {
      Print(pRepOut," -w");
    }
    Print(pRepOut,"\n");
    PrintString(pRepOut,"    %s\n\n",aAlgorithms[j].sDescription);
  }
  AbnormalExit();
}

void HelpPrintReports() {
  unsigned int j,k;

  PrintUBCSATHeader(pRepOut);
  Print(pRepOut,"\nPrinting Reports in UBCSAT:\n");
  Print(pRepOut,"--------------------------\n\n");

  Print(pRepOut,"To specify a report use the following syntax:\n");
  Print(pRepOut,"  -r reportname [filename [paramater(s)]] \n\n");
  
  Print(pRepOut,"If you do not specify a filename or parameter(s), defaults will be used.\n\n");
  Print(pRepOut,"For filename, you may alternatively specify the following special keywords:\n");
  Print(pRepOut,"  stdout (default), stderr, or null\n\n");

  Print(pRepOut,"REPORTS:\n\n");
  
  for (j=0;j<iNumReports;j++) {
    PrintString(pRepOut, "  -r %s\n",aReports[j].sID);
    PrintString(pRepOut,"    %s\n",aReports[j].sDescription);
  
    if (aReports[j].iNumParms) {

      for (k=0;k<aReports[j].iNumParms;k++) {
        PrintUInt(pRepOut,"      Parameter %2d: ",k+1);
        PrintString(pRepOut,"%s",aReports[j].aParmName[k]);
        switch (aReports[j].aParmTypes[k]) {
          {
          case TypeInt:
            PrintSInt(pRepOut," [%d] \n",*(int *)aReports[j].aParameters[k]);
            break;
          case TypeString:
            PrintString(pRepOut," [%s] \n",(char *)aReports[j].aParameters[k]);          
            break;
	  default:
	    break;
          }
        }
      }
    }
    Print(pRepOut,"\n");
  }

  AbnormalExit();
}

void HelpPrintColumns() {
  unsigned int j;

  PrintUBCSATHeader(pRepOut);
  Print(pRepOut,"\nPrinting Reports with Columns in UBCSAT:\n");
  Print(pRepOut,  "---------------------------------------\n\n");

  Print(pRepOut,"For Output & RTD reports, you may customize the report by specifying\n");
  Print(pRepOut,"which columns you wish to display.  For example:\n");
  Print(pRepOut,"  -r out myfile.out found,beststep\n");
  Print(pRepOut,"displays only the output columns found & beststep to the file myfile.out\n");

  Print(pRepOut,"\n\nOutput (-r out) & RTD (-r rtd) Columns available:\n\n");

  for (j=0;j<listColumns.iNumItems;j++) {
    if (!listColumns.aItems[j].bCompound) {
      PrintString(pRepOut,"%20s -  ",listColumns.aItems[j].sID);
      PrintString(pRepOut,"%s\n",aColumns[j].sDescription);
    }
  }

  Print(pRepOut,"\nCompound Columns available:\n\n");

  for (j=0;j<listColumns.iNumItems;j++) {
    if (listColumns.aItems[j].bCompound) {
      PrintString(pRepOut,"%20s -  ",listColumns.aItems[j].sID);
      PrintString(pRepOut,"%s\n",listColumns.aItems[j].sCompoundList);
    }
  }


  AbnormalExit();
}

void HelpPrintStats() {
  unsigned int j;

  PrintUBCSATHeader(pRepOut);
  Print(pRepOut,"\nPrinting Statistics Reports in UBCSAT:\n");
  Print(pRepOut,  "-------------------------------------\n\n");

  Print(pRepOut,"For statistics reports, you may customize the report by specifying\n");
  Print(pRepOut,"which statistics you wish to display.  For example:\n\n");
  Print(pRepOut,"  -r stats mystats.stats percentsolve,steps[mean;median;max],numlits\n\n");
  Print(pRepOut,"writes to file mystats.stats the following lines:\n\n");
  
  Print(pRepOut,"PercentSuccess = x.x\n");
  Print(pRepOut,"Steps_Mean = x.x\n");
  Print(pRepOut,"Steps_Median = x.x\n");
  Print(pRepOut,"Steps_Max = x.x\n");
  Print(pRepOut,"TotalLiterals = x\n\n");

  Print(pRepOut,"For full statistics, you can specify which fields to print from:\n");
  Print(pRepOut,"  [");
  for (j=0;j<NUMVALIDSTATCODES;j++) {
    if (j>0)
      Print(pRepOut,";");
    PrintString(pRepOut,"%s",sValidStatCodes[j]);
  }
  Print(pRepOut,"]\n\n");

  Print(pRepOut,"Full Statistics available: [defaults]\n\n");
  for (j=0;j<listStats.iNumItems;j++) {
    if (!listStats.aItems[j].bCompound) {
      if (!aStats[j].bCustomField) {
        PrintString(pRepOut,"%20s -  ",listStats.aItems[j].sID);
        PrintString(pRepOut,"%s",aStats[j].sDescription);
        PrintString(pRepOut,"  [%s]\n",aStats[j].sStatParms);
      }
    }
  }

  Print(pRepOut,"\n\nSingle Statistics available:\n\n");

  for (j=0;j<listStats.iNumItems;j++) {
    if (!listStats.aItems[j].bCompound) {
      if (aStats[j].bCustomField) {
        PrintString(pRepOut,"%20s -  ",listStats.aItems[j].sID);
        PrintString(pRepOut,"%s\n",aStats[j].sDescription);
      }
    }
  }


  Print(pRepOut,"\nCompound Statistics available:\n\n");

  for (j=0;j<listStats.iNumItems;j++) {
    if (listStats.aItems[j].bCompound) {
      PrintString(pRepOut,"%20s -  ",listStats.aItems[j].sID);
      PrintString(pRepOut,"%s\n",listStats.aItems[j].sCompoundList);
    }
  }


  AbnormalExit();
}



void HelpNoAlgorithm() {

  PrintUBCSATHeader(pRepErr);
  Print(pRepErr,"\n");
  Print(pRepErr,"FATAL ERROR ENCOUNTERED: \n");

  if (*sAlgName) {
    Print(pRepErr,"   Invalid algorithm specified\n");
  } else {
    Print(pRepErr,"   UBCSAT requires you to specify an algorithm with -alg\n");
  }
  Print(pRepErr,"   ubcsat -ha for a list of valid algorithms\n");
  AbnormalExit();
}


void HelpPrintParameters(ALGPARMLIST *pParmList) {
  
  unsigned int j;
  ALGPARM *pCurParm;

  if (pParmList->iNumParms==0) {
    Print(pRepOut,"  No Paramaters\n\n");
  } else {
    for (j=0;j<pParmList->iNumParms;j++) {
      pCurParm = &pParmList->aParms[j];
      PrintString(pRepOut,"  %s: \n",pCurParm->sName);
      PrintString(pRepOut,"    %s\n",pCurParm->sDescription);
      PrintString(pRepOut,"    %s ",pCurParm->sSwitch);
      switch(pCurParm->eType)
      {
        case TypeInt:
          if (strcmp(pCurParm->sName,"seed")==0) {
            PrintUInt(pRepOut,"N (default based on system time) ",pCurParm->defDefault1.iInt);
          } else {
            PrintUInt(pRepOut,"N (default %d) ",pCurParm->defDefault1.iInt);
          }
          break;
        case TypeSInt:
          PrintSInt(pRepOut,"N (default %d) ",pCurParm->defDefault1.iSInt);
          break;
        case TypeRatio:
          PrintUInt(pRepOut,"N [M] (defaults are %d",pCurParm->defDefault1.iInt);
          PrintUInt(pRepOut,"/%d)",pCurParm->defDefault2.iInt);
          break;
        case TypeString:
          Print(pRepOut,"name ");
          break;
        case TypeFloat:
          PrintFloat(pRepOut,"n.nn (default %f) ",pCurParm->defDefault1.fFloat);
          break;
        case TypeReport:
          Print(pRepOut,"reportname [filename [parameters]]");
          break;
        default:
	  break;
      }  
      Print(pRepOut,"\n");
    }
    Print(pRepOut,"\n\n");
  }
}

void HelpPrintParametersTerse(ALGPARMLIST *pParmList) {
  
  unsigned int j;
  ALGPARM *pCurParm;

  Print(pRepOut,"  ");
  for (j=0;j<pParmList->iNumParms;j++) {
    pCurParm = &pParmList->aParms[j];
    PrintString(pRepOut,"%s ",pCurParm->sSwitch);
  }
  Print(pRepOut,"\n");
}



void HelpPrintAlgParameters(ALGORITHM *pCurAlg) {

  if (pCurAlg) {
    if (strcmp(pCurAlg->sName,"default")==0) {
      PrintString(pRepOut,"[-alg %s",pCurAlg->sName);
      if (*pCurAlg->sVariant != 0)
        PrintString(pRepOut," -v %s",pCurAlg->sVariant);
      if (pCurAlg->bWeighted)
        Print(pRepOut," -w");
      Print(pRepOut,"]\n");
    }
    PrintString(pRepOut,"%s\n\n",pCurAlg->sDescription);
    HelpPrintParameters(&pCurAlg->parmList);
  }
}


void HelpPrintAlgParametersTerse(ALGORITHM *pCurAlg) {

  if (strcmp(pCurAlg->sName,"default")!=0) {
    PrintString(pRepOut,"[-alg %s",pCurAlg->sName);
    if (*pCurAlg->sVariant != 0)
      PrintString(pRepOut," -v %s",pCurAlg->sVariant);
    if (pCurAlg->bWeighted)
      Print(pRepOut," -w");
    Print(pRepOut,"]\n");
  }
  HelpPrintParametersTerse(&pCurAlg->parmList);
}

void HelpPrintSpecialParameters() {

  Print(pRepOut,"Help Parameters:\n");
  HelpPrintParameters(&parmHelp);

  Print(pRepOut,"Algorithm Specification Parameters:\n");
  HelpPrintParameters(&parmAlg);
  
  Print(pRepOut,"UBCSAT Reporting & File I/O Parameters:\n");
  HelpPrintParameters(&parmIO);  

  Print(pRepOut,"UBCSAT Parameters:\n");
  HelpPrintParameters(&parmUBCSAT);

}

void HelpPrintSpecialParametersTerse() {

  Print(pRepOut,"Help Parameters:\n");
  HelpPrintParametersTerse(&parmHelp);

  Print(pRepOut,"Algorithm Specification Parameters:\n");
  HelpPrintParametersTerse(&parmAlg);
  
  Print(pRepOut,"UBCSAT Parameters:\n");
  HelpPrintParametersTerse(&parmUBCSAT);

  Print(pRepOut,"UBCSAT Reporting & File I/O Parameters:\n");
  HelpPrintParametersTerse(&parmIO);

}


void HelpShow() {

  HelpPrintSpecialParameters();
  HelpPrintAlgParameters(pAlg);

  AbnormalExit();
}

void HelpShowV() {
  unsigned int j;
  HelpPrintSpecialParameters();
  for (j=0;j<iNumAlg;j++)
    HelpPrintAlgParameters(&aAlgorithms[j]);
  AbnormalExit();
}

void HelpShowT() {
  unsigned int j;
  HelpPrintSpecialParametersTerse();
  for (j=0;j<iNumAlg;j++)
    HelpPrintAlgParametersTerse(&aAlgorithms[j]);
  AbnormalExit();
}

void HelpBadParm(char *sParm) {
  PrintUBCSATHeader(pRepErr);
  Print(pRepErr,"\n");
  Print(pRepErr,"FATAL ERROR ENCOUNTERED: \n");
  PrintString(pRepErr,"   Parameter [%s] is unrecognized or invalid\n",sParm);
  Print(pRepErr,"\n");
  AbnormalExit();
}


void HelpBadReport(char *sParm) {
  PrintUBCSATHeader(pRepErr);
  Print(pRepErr,"\n");
  Print(pRepErr,"FATAL ERROR ENCOUNTERED: \n");
  PrintString(pRepErr,"   Report Parameter [%s] is unrecognized\n",sParm);
  Print(pRepErr,"   ubcsat -hr for help on report parameters  \n");
  Print(pRepErr,"\n");
  AbnormalExit();
}

