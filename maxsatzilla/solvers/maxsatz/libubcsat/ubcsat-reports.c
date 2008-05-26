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

char sMasterString[MAXITEMLISTSTRINGLENGTH];

char sDefaultComment[] = "c";

char *sCommentString = sDefaultComment;

unsigned int iNumReports = 0;
REPORT aReports[MAXREPORTS];

ITEMLIST listColumns = { 0 };
ITEMLIST listStats = { 0 };

REPORTCOL aColumns[MAXITEMLIST];
REPORTSTAT aStats[MAXITEMLIST];
unsigned int iNumStatsActive = 0;

unsigned int *aSortedBySteps;
unsigned int *aStepArray;

unsigned int *aFlipCount;
unsigned int *aUnsatCount;
signed int *aClauseLast;

FLOAT fDummy = 0.0;

FLOAT fPercentSuccess;
FLOAT fFlipsPerSecond;
FLOAT fPercentNullFlips;

unsigned int iNumOutputColumns = 0;
unsigned int aOutputColumns[MAXITEMLIST];

unsigned int iNumRTDColumns = 0;
unsigned int aRTDColumns[MAXITEMLIST];

unsigned int iTracePenMaxLM = 0;

char *sValidStatCodes[NUMVALIDSTATCODES] = {"all","mean","stddev","cv","median","min","max","q05","q10","q25","q75","q90","q95","q98","qr75/25","qr90/10","qr95/05"};



/**** 0.9.8m changes */

unsigned int iStartFalse;

void SetStartFalse() {
  iStartFalse = iNumFalse;
}

FLOAT fImproveMean;

void CalcImproveMean() {
  if (iBestStepNumFalse==0) {
    fImproveMean = 0.0f;
  } else {
    fImproveMean = (FLOAT) (iStartFalse - iBestNumFalse) / (FLOAT) (iBestStepNumFalse);
  }
}

#define MAXBEST 1000
/* temp for now... assume no more than 1000 best improvements */
unsigned int *aBestFalseTrack;
unsigned int *aBestFalseStepTrack;
unsigned int iNumBestFalseTrack;
unsigned int iFirstLMFalse;

FLOAT  fFirstLMRatio;

FLOAT fBestFalseTrackMean;
FLOAT fBestFalseTrackCV;

void CreateBestTrack() {
  aBestFalseTrack = (unsigned int*)AllocateRAM(MAXBEST*sizeof(unsigned int));
  aBestFalseStepTrack = (unsigned int*)AllocateRAM(MAXBEST*sizeof(unsigned int));
}

void InitBestTrack() {
  iNumBestFalseTrack = 0;
}

void UpdateBestTrack() {
  if (iStep == 1) {
    aBestFalseTrack[0] = iNumFalse;
    aBestFalseStepTrack[0] = 1;
    iNumBestFalseTrack = 1;
    iFirstLMFalse = iNumFalse;
  } else {
    if (iBestStepNumFalse == iStep) {
      if (aBestFalseStepTrack[iNumBestFalseTrack-1] == (iStep - 1)) {
        aBestFalseTrack[iNumBestFalseTrack-1] = iNumFalse;
        aBestFalseStepTrack[iNumBestFalseTrack-1] = iStep;
        if (iNumBestFalseTrack == 1) {
          iFirstLMFalse = iNumFalse;
        }
      } else {
        aBestFalseTrack[iNumBestFalseTrack] = iNumFalse;
        aBestFalseStepTrack[iNumBestFalseTrack] = iStep;
        iNumBestFalseTrack++;
      }
    }
  }
}

void FinalBestTrack() {
  unsigned int j;
  FLOAT fStdDev;
  FLOAT fTemp;

  fBestFalseTrackMean = 0.0f;
  fStdDev = 0.0f;
  fBestFalseTrackCV = 0.0f;
  if (iNumBestFalseTrack > 0)  {
    for (j=0;j<iNumBestFalseTrack;j++) {
      fBestFalseTrackMean += (FLOAT) aBestFalseTrack[j];
    }
    fBestFalseTrackMean /= (FLOAT) iNumBestFalseTrack;

    if (iNumBestFalseTrack > 1) {
      for (j = 0; j < iNumBestFalseTrack; j++) {
        fTemp = (FLOAT) aBestFalseTrack[j];
        fTemp -= fBestFalseTrackMean;
        fStdDev += (fTemp*fTemp);
      }
      fStdDev /= (FLOAT) (iNumBestFalseTrack-1);
      fStdDev = sqrt(fStdDev);
    }

    if (fBestFalseTrackMean > 0) {
      fBestFalseTrackCV = fStdDev / fBestFalseTrackMean;
    }
  }

  if(iStartFalse != iBestNumFalse)
    {
      fFirstLMRatio = (FLOAT)(iStartFalse - iFirstLMFalse) / (FLOAT)(iStartFalse - iBestNumFalse);
    }
  else
    {
      fFirstLMRatio = 1000.00;
    }

}


/* 0.9.8m changes ****/









void AddOutputColumn(unsigned int j, const char *sItem) {
  NOREF(sItem);
  aOutputColumns[iNumOutputColumns++] = j;
}

void SetupOutput() {
  iNumOutputColumns = 0;
  ParseItemList(&listColumns,(char *)pRepOut->aParameters[0],ActivateColumn);
  ParseItemList(&listColumns,(char *)pRepOut->aParameters[0],AddOutputColumn);
}

void AddRTDColumn(unsigned int j, const char *sItem) {
  NOREF(sItem);
  aRTDColumns[iNumRTDColumns++] = j;
}

void SetupRTD() {
  iNumRTDColumns = 0;
  ParseItemList(&listColumns,(char *)pRepRTD->aParameters[0],ActivateColumn);
  ParseItemList(&listColumns,(char *)pRepRTD->aParameters[0],AddRTDColumn);
}

void SetupStats() {
  if(pRepStats -> bActive)
    ParseItemList(&listStats,(char *)pRepStats->aParameters[0],ActivateStat);
  if(pRepSatzilla -> bActive)
    ParseItemList(&listStats,(char *)pRepSatzilla->aParameters[0],ActivateStat);
}

void AddItem(ITEMLIST *pList,const char *sID) {
  SetString(&pList->aItems[pList->iNumItems].sID,sID);
  pList->aItems[pList->iNumItems].bCompound = 0;
  pList->iNumItems++;
  if (pList->iNumItems==MAXITEMLIST) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXITEMLIST [%d] \n",MAXITEMLIST);
    AbnormalExit();
  }
}

void AddCompoundItem(ITEMLIST *pList,const char *sID, const char *sList) {
  SetString(&pList->aItems[pList->iNumItems].sID,sID);
  pList->aItems[pList->iNumItems].bCompound = 1;
  SetString(&pList->aItems[pList->iNumItems].sCompoundList,sList);
  pList->iNumItems++;
  if (pList->iNumItems==MAXITEMLIST) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXITEMLIST [%d] \n",MAXITEMLIST);
    AbnormalExit();
  }
}

unsigned int FindItem(ITEMLIST *pList,char *sID) {
  unsigned int j;
  char *pPos;
  char *pPos2;
  unsigned int iLen;

  pPos = strchr(sID,'[');
  if (pPos) {
    pPos2 = strchr(sID,']');
    if (pPos2) {
      iLen = pPos - sID;
      for (j=0;j<pList->iNumItems;j++) {
        if (strlen(pList->aItems[j].sID) == iLen) {
          if (strncmp(sID,pList->aItems[j].sID,iLen)==0) {
            return(j);
          }
        }
      }
    } else {
      PrintString(pRepErr,"Error: unbalanced [] in (%s)\n",sID);
    }
  } else {
    for (j=0;j<pList->iNumItems;j++) {
        if (strcmp(sID,pList->aItems[j].sID)==0) {
          return(j);
        }
    }
  }
  PrintString(pRepErr,"Error: reference to (%s) is unknown\n",sID);
  AbnormalExit();
  return(0);
}

void ParseItemList(ITEMLIST *pList, char *sItems, CALLBACKPTR ItemFunction) {

  char *pPos;
  signed int j;

  if (*sItems==0)
    return;

  pPos = strchr(sItems,',');
  if (pPos) {
    if (strlen(sItems) > MAXITEMLISTSTRINGLENGTH-1) {
      PrintUInt(pRepErr,"Unexpected Error: increase constant MAXITEMLISTSTRINGLENGTH [%d] \n",MAXITEMLISTSTRINGLENGTH);
      AbnormalExit();
    }
    strcpy(sMasterString,sItems);
    sMasterString[pPos-sItems] = 0;
    pPos++;
    ParseItemList(pList,sMasterString,ItemFunction);
    ParseItemList(pList,pPos,ItemFunction);
    return;
  }
  j = FindItem(pList,sItems);
  if (pList->aItems[j].bCompound) {
    ParseItemList(pList,pList->aItems[j].sCompoundList,ItemFunction);
  } else {
    ItemFunction((unsigned int) j,sItems);
  }
}


REPORT *AddReport(const char *sID, const char *sDescription, const char *sOutputFile, const char *sFunctions) {
  REPORT *pRep;

  pRep = &aReports[iNumReports++];

  if (iNumReports==MAXREPORTS) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXREPORTS [%d] \n",MAXREPORTS);
    AbnormalExit();
  }


  SetString(&pRep->sID,sID);
  pRep->bActive = 0;
  pRep->fileOut = stdout;
  SetString(&pRep->sOutputFile,sOutputFile);
  pRep->iNumParms = 0;
  SetString(&pRep->sFunctions,sFunctions);
  SetString(&pRep->sDescription,sDescription);

  return (pRep);
}

void AddReportParm(REPORT *pRep, const char *sParmName, enum PARMTYPE eType, void *pDefault) {
  
  SetString(&pRep->aParmName[pRep->iNumParms],sParmName);

  pRep->aParmTypes[pRep->iNumParms] = eType;
  
  switch (eType)
  {
    case TypeInt:
    case TypeSInt:
    case TypeBool:
    case TypeFloat:
      pRep->aParameters[pRep->iNumParms] = pDefault;
      break;
    case TypeString:
      SetString((char **) &pRep->aParameters[pRep->iNumParms],(const char *) pDefault);
      break;
    case TypeReport:
    case TypeRatio:
      break;
  }
  pRep->iNumParms++;
  if (pRep->iNumParms==MAXREPORTPARMS) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXREPORTPARMS [%d]\n",MAXREPORTPARMS);
    AbnormalExit();
  }

}
void VerifyStatsParms (char *sStatsParms) {

  unsigned int j;
  char *pPos;
  char *pPos2;
  unsigned int iLen;
  unsigned int bValid;
  
  pPos = sStatsParms;

  while (*pPos != 0) {
    bValid = 0;
    pPos2 = strstr(pPos,";");

    if (pPos2) {
      iLen = pPos2 - pPos;
    } else {
      iLen = strlen(pPos);
    }
    
    if (iLen) {
      for (j=0;j<NUMVALIDSTATCODES;j++) {
        if (strlen(sValidStatCodes[j])==iLen) {
          if (strncmp(pPos,sValidStatCodes[j],iLen)==0) {
            bValid = 1;  
          }
        }
      }
      if (bValid==0) {
        PrintString(pRepErr,"Error: reference to (%s) is unknown\n",pPos);        
        AbnormalExit();
      }
    }

    pPos += iLen;
    if (pPos2)
      pPos++;
  }
}

void ActivateColumn(unsigned int iColID, const char *sItem) {
  
  REPORTCOL *pCol;
  NOREF(sItem);
 
  pCol = &aColumns[iColID];

  if (pCol->bActive == 0) {
  
    pCol->bActive = 1;

    ParseItemList(&listFunctions,pCol->sFunctions,ActivateFunction);

    switch (pCol->eType) {
      case ColInt:
        pCol->pColumnData = AllocateRAM(iNumRuns * sizeof(unsigned int));
        memset(pCol->pColumnData,0,(iNumRuns)*sizeof(unsigned int));
        break;
      case ColFloat:
        pCol->pColumnData = AllocateRAM(iNumRuns * sizeof(FLOAT));
        memset(pCol->pColumnData,0,(iNumRuns)*sizeof(FLOAT));
        break;
    }
  }
}

void ActivateStat(unsigned int iStatID, const char *sItem) {

  REPORTSTAT *pStat;
  char *pPos;
  char *pPos2;

  pStat = &aStats[iStatID];

  if (pStat->bActive == 0) {
    pStat->bActive = 1;
    pStat->iActiveID = iNumStatsActive++;

    pPos = strchr(sItem,'[');
    if (pPos) {
      pPos++;
      pPos2 = strchr(pPos,']');
      if (pPos2) {
        *(pPos2) = 0;
        SetString(&pStat->sStatParms,pPos);
      }
    }

    if ((pStat->bCustomField==0)&&(pStat->bCustomField==0)) {
      VerifyStatsParms(pStat->sStatParms);    
    }
  
    ParseItemList(&listColumns,pStat->sRequiredCols,ActivateColumn);
    ParseItemList(&listFunctions,pStat->sFunctions,ActivateFunction);
  }
}

int CompareSorted(const void *a, const void *b) {
  return (aStepArray[*(unsigned int *)a] - aStepArray[*(unsigned int *)b]);
}

void SortByStepPerformance() {

  unsigned int j;
  REPORTCOL *pCol;

  pCol = &aColumns[FindItem(&listColumns,"steps")];

  aStepArray = (unsigned int *)pCol->pColumnData;

  aSortedBySteps = (unsigned int *)AllocateRAM(sizeof(unsigned int) * iRun);

  for (j=0;j<iRun;j++) {
    aSortedBySteps[j] = j;
  }

  qsort((void *)aSortedBySteps,iRun,sizeof(unsigned int),CompareSorted);

}

void UpdatePercents() {
  
  unsigned int j;
  REPORTCOL *pCol;
  FLOAT *aPercents=0;

  pCol = &aColumns[FindItem(&listColumns,"prob")];
  aPercents = (double *)pCol->pColumnData;
  for (j=0;j<iRun;j++) {
    aPercents[aSortedBySteps[j]] = ((FLOAT) (j+1))/((FLOAT)iRun);
  }

}

void UpdateTimes() {
  
  unsigned int j;
  REPORTCOL *pColTimes;
  REPORTCOL *pColSteps;
  FLOAT *aTimes;
  unsigned int *aSteps;

  FLOAT fTotalSteps = 0.0;

  pColSteps = &aColumns[FindItem(&listColumns,"steps")];

  aSteps = (unsigned int *) pColSteps->pColumnData;
  for (j=0;j<iRun;j++) {
    fTotalSteps += (FLOAT) aSteps[j];
  }

  pColTimes = &aColumns[FindItem(&listColumns,"time")];
  aTimes = (FLOAT *) pColTimes->pColumnData;
  for (j=0;j<iRun;j++) {
    aTimes[j] = (FLOAT) aSteps[j];
    aTimes[j] /= fTotalSteps;
    aTimes[j] *= fTotalTime;
  }

}


void AddColumnInt(const char *sID, 
                  const char *sDescription, 
                  unsigned int iCharWidth, 
                  char *sHeader1,  
                  char *sHeader2,  
                  char *sHeader3, 
                  char *sPrintFormat, 
                  unsigned int *pCurValue,
                  char *sFunctions
                  )
{

  REPORTCOL *pCol;
  
  pCol = &aColumns[listColumns.iNumItems];

  pCol->bActive = 0;

  SetString(&pCol->sDescription,sDescription);
  pCol->iCharWidth = iCharWidth;
  SetString(&pCol->sHeader1,sHeader1);
  SetString(&pCol->sHeader2,sHeader2);
  SetString(&pCol->sHeader3,sHeader3);
  SetString(&pCol->sPrintFormat,sPrintFormat);
  pCol->pCurValue = (void *) pCurValue;

  pCol->eType = ColInt;

  SetString(&pCol->sFunctions,sFunctions);
  
  pCol->pColumnData = 0;

  AddItem(&listColumns,sID);
}


void AddColumnFloat(const char *sID, 
                  const char *sDescription, 
                  unsigned int iCharWidth, 
                  char *sHeader1,  
                  char *sHeader2,  
                  char *sHeader3, 
                  char *sPrintFormat,
                  FLOAT *pCurValue,
                  char *sFunctions
                  )
{

  REPORTCOL *pCol;
  
  pCol = &aColumns[listColumns.iNumItems];

  pCol->bActive = 0;

  SetString(&pCol->sDescription,sDescription);
  pCol->iCharWidth = iCharWidth;
  SetString(&pCol->sHeader1,sHeader1);
  SetString(&pCol->sHeader2,sHeader2);
  SetString(&pCol->sHeader3,sHeader3);
  SetString(&pCol->sPrintFormat,sPrintFormat);
  pCol->pCurValue = (void *) pCurValue;

  pCol->eType = ColFloat;

  SetString(&pCol->sFunctions,sFunctions);

  pCol->pColumnData = 0;

  AddItem(&listColumns,sID);

}

void AddColumnComposite(const char *sID, 
                        const char *sList)
{
  AddCompoundItem(&listColumns,sID,sList);
}


void AddStat(const char *sID, 
                  const char *sDescription, 
                  const char *sBaseDescription, 
                  const char *sDefParm,
                  const char *sRequiredCols,
                  const char *sDataColumn,
                  const char *sFunctions
                )
{

  REPORTSTAT *pStat;

  pStat = &aStats[listStats.iNumItems];

  pStat->bActive = 0;

  SetString(&pStat->sDescription,sDescription);
  
  pStat->bCustomField = 0;

  SetString(&pStat->sBaseDescription,sBaseDescription);
  
  SetString(&pStat->sRequiredCols,sRequiredCols);
  SetString(&pStat->sDataColumn,sDataColumn);
  SetString(&pStat->sStatParms,sDefParm);

  SetString(&pStat->sFunctions,sFunctions);

  AddItem(&listStats,sID);
}

void AddStatComposite(const char *sID, 
                      const char *sList) {

  AddCompoundItem(&listStats,sID,sList);
}

void AddStatCustom(const char *sID, 
                  const char *sDescription, 
                  const char *sPrintCustomFormat,
                  void *pCurValue,
                  enum COLTYPE eCustomType,
                  const char *sRequiredCols,
                  const char *sFunctions
                )
{

  REPORTSTAT *pStat;

  pStat = &aStats[listStats.iNumItems];

  pStat->bActive = 0;

  SetString(&pStat->sDescription,sDescription);

  SetString(&pStat->sBaseDescription,sID);
  
  pStat->bCustomField = 1;

  SetString(&pStat->sPrintCustomFormat,sPrintCustomFormat);
  
  pStat->pCustomValue = pCurValue;
  pStat->eCustomType = eCustomType;

  SetString(&pStat->sRequiredCols,sRequiredCols);

  SetString(&pStat->sFunctions,sFunctions);
  
  AddItem(&listStats,sID);

}

FLOAT GetRowElement(REPORTCOL *pCol,unsigned int iRow) {

  FLOAT fReturn = 0.0;
  
  int *aInt;
  FLOAT *aFloat;

  switch(pCol->eType)
  {
  case ColInt:
    aInt = (int *) pCol->pColumnData;
    fReturn = (FLOAT) aInt[iRow];
    break;
  case ColFloat:
    aFloat = (FLOAT *) pCol->pColumnData;
    fReturn = aFloat[iRow];
    break;
  }

  return(fReturn);
}

void PrintFullStat(REPORTSTAT *pStat, const char *sStatID, char *sPrintID, FLOAT fValue) {
  if ((strstr(pStat->sStatParms,sStatID))||(strstr(pStat->sStatParms,"all"))) { 
    PrintString(pRepStats,"%s",pStat->sBaseDescription);
    PrintString(pRepStats,"_%s = ",sPrintID);
    PrintFloat(pRepStats,"%f\n",fValue);
  }
}


void PrintStats() {
  
  unsigned int j,k;
  unsigned int iRow;
  REPORTCOL *pCol;
  REPORTSTAT *pStat;
  
  FLOAT fMean;
  FLOAT fMedian;
  FLOAT fStdDev;
  FLOAT fTemp;

  Print(pRepStats,"\n\n");

  for (k=0;k<iNumStatsActive;k++) {
    for (j=0;j<listStats.iNumItems;j++) {
      pStat = &aStats[j];
      if ((pStat->bActive)&&(pStat->iActiveID==k)) {
        if (pStat->bCustomField) {
          switch (pStat->eCustomType) {
            case ColInt:
              PrintUInt(pRepStats,pStat->sPrintCustomFormat,*(unsigned int *) pStat->pCustomValue);
              break;
            case ColFloat:
              PrintFloat(pRepStats,pStat->sPrintCustomFormat,*(FLOAT *) pStat->pCustomValue);
              break;
          }
        } else {

          pCol = &aColumns[FindItem(&listColumns,pStat->sDataColumn)];

          if (iRun) {

            fMean = 0.0; 
            for (iRow = 0; iRow < iRun; iRow++) {
              fMean += GetRowElement(pCol,iRow);
            }
            fMean /= (FLOAT) iRun;

            fStdDev = 0.0; 
            if (iRun > 1) {
              for (iRow = 0; iRow < iRun; iRow++) {
                fTemp = GetRowElement(pCol,iRow)-fMean;
                fStdDev += (fTemp*fTemp);
              }
              fStdDev /= (FLOAT) (iRun-1);
              fStdDev = sqrt(fStdDev);
            }

            fMedian = GetRowElement(pCol,aSortedBySteps[(iRun-1)>>1]);
            if (iRun % 2 == 0) {
              fMedian += GetRowElement(pCol,aSortedBySteps[(iRun)>>1]);
              fMedian /= 2.0;
            }

            PrintFullStat(pStat,"mean","Mean",fMean);
            PrintFullStat(pStat,"stddev","StdDev",fStdDev);
            PrintFullStat(pStat,"cv","CoeffVariance",fStdDev / fMean);
            PrintFullStat(pStat,"median","Median",fMedian);
            PrintFullStat(pStat,"min","Min",GetRowElement(pCol,aSortedBySteps[0]));
            PrintFullStat(pStat,"max","Max",GetRowElement(pCol,aSortedBySteps[iRun-1]));
            PrintFullStat(pStat,"q05","Q.05",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.05 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q10","Q.10",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.10 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q25","Q.25",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.25 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q75","Q.75",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.75 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q90","Q.90",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.90 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q95","Q.95",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.95 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"q98","Q.98",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.98 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"qr75/25","Q.75/25",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.75 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.25 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"qr90/10","Q.90/10",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.90 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.10 * (FLOAT) (iRun-1)))]));
            PrintFullStat(pStat,"qr95/05","Q.95/05",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.95 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.05 * (FLOAT) (iRun-1)))]));
            
          }
        }
      }
    }
  }
  Print(pRepStats,"\n\n");
}

void writeFeature(char* name, FLOAT value) {
  printf("feature %s: %f\n", name, value);
}

void PrintSatzillaStat(REPORTSTAT *pStat, const char *sStatID, char *sPrintID, FLOAT fValue) {
  char buf[256];

  if(bSolveMode && !bSatzillaMode)
    return;

  if(sStatID==NULL)
    {
      sprintf(buf, "%s_%s", sAlgName, sPrintID);
      writeFeature(buf, fValue);
      return;
    }

  if ((strstr(pStat->sStatParms,sStatID))||(strstr(pStat->sStatParms,"all"))) { 
    sprintf(buf, "%s_%s_%s", sAlgName, pStat->sBaseDescription, sPrintID);
    writeFeature(buf, fValue);

  }
}

void SatzillaPrintStats() {
  
  unsigned int j,k;
  unsigned int iRow;
  REPORTCOL *pCol;
  REPORTSTAT *pStat;
  
  FLOAT fMean;
  FLOAT fMedian;
  FLOAT fStdDev;
  FLOAT fTemp;

  /*  Print(pRepStats,"\n\n");*/

  for (k=0;k<iNumStatsActive;k++) {
    for (j=0;j<listStats.iNumItems;j++) {
      pStat = &aStats[j];
      if ((pStat->bActive)&&(pStat->iActiveID==k)) {
        if (pStat->bCustomField) {
          switch (pStat->eCustomType) {
            case ColInt:
	      PrintSatzillaStat(NULL, NULL, pStat->sBaseDescription,
				(FLOAT)(*(unsigned int *) pStat->pCustomValue));
              break;
            case ColFloat:
	      PrintSatzillaStat(NULL, NULL, pStat->sBaseDescription,
				*(FLOAT *) pStat->pCustomValue);
              break;
          }
        } else {

          pCol = &aColumns[FindItem(&listColumns,pStat->sDataColumn)];

          if (iRun) {

            fMean = 0.0; 
            for (iRow = 0; iRow < iRun; iRow++) {
              fMean += GetRowElement(pCol,iRow);
            }
            fMean /= (FLOAT) iRun;

            fStdDev = 0.0; 
            if (iRun > 1) {
              for (iRow = 0; iRow < iRun; iRow++) {
                fTemp = GetRowElement(pCol,iRow)-fMean;
                fStdDev += (fTemp*fTemp);
              }
              fStdDev /= (FLOAT) (iRun-1);
              fStdDev = sqrt(fStdDev);
            }

            fMedian = GetRowElement(pCol,aSortedBySteps[(iRun-1)>>1]);
            if (iRun % 2 == 0) {
              fMedian += GetRowElement(pCol,aSortedBySteps[(iRun)>>1]);
              fMedian /= 2.0;
            }

            PrintSatzillaStat(pStat,"mean","Mean",fMean);
            PrintSatzillaStat(pStat,"stddev","StdDev",fStdDev);
            PrintSatzillaStat(pStat,"cv","CoeffVariance",(fMean !=0.0 ? fStdDev / fMean : 100.0) );
            PrintSatzillaStat(pStat,"median","Median",fMedian);
            PrintSatzillaStat(pStat,"min","Min",GetRowElement(pCol,aSortedBySteps[0]));
            PrintSatzillaStat(pStat,"max","Max",GetRowElement(pCol,aSortedBySteps[iRun-1]));
            PrintSatzillaStat(pStat,"q05","Q.05",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.05 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q10","Q.10",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.10 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q25","Q.25",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.25 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q75","Q.75",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.75 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q90","Q.90",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.90 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q95","Q.95",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.95 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"q98","Q.98",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.98 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"qr75/25","Q.75/25",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.75 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.25 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"qr90/10","Q.90/10",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.90 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.10 * (FLOAT) (iRun-1)))]));
            PrintSatzillaStat(pStat,"qr95/05","Q.95/05",GetRowElement(pCol,aSortedBySteps[(int)(floor(0.95 * (FLOAT) (iRun-1)))])/GetRowElement(pCol,aSortedBySteps[(int)(floor(0.05 * (FLOAT) (iRun-1)))]));
            
          }
        }
      }
    }
  }
  /*  Print(pRepStats,"\n\n");*/
}


void PrintColHeaders(REPORT *pRep,unsigned int iNumCols, unsigned int *aCols) {

  unsigned int j;
  if (!bReportClean) {

    PrintHdr(pRep,1,"\n");
    PrintHdr(pRep,1,"Output Columns: ");
    for (j=0;j<iNumCols;j++) {
      PrintHdrString(pRep,0,"|%s",listColumns.aItems[aCols[j]].sID);
    }
    PrintHdr(pRep,0,"|\n");
    PrintHdr(pRep,1,"\n");
    for (j=0;j<iNumCols;j++) {
      PrintHdrString(pRep,1,"%s: ",listColumns.aItems[aCols[j]].sID);
      PrintHdrString(pRep,0,"%s\n",aColumns[aCols[j]].sDescription);
    }
    PrintHdr(pRep,1,"\n");
    PrintHdr(pRep,1,"");
    for (j=0;j<iNumCols;j++) {
      PrintHdr(pRep,0,aColumns[aCols[j]].sHeader1);
      PrintHdr(pRep,0," ");
    }
    PrintHdr(pRep,0,"\n");
    PrintHdr(pRep,1,"");
    for (j=0;j<iNumCols;j++) {
      PrintHdr(pRep,0,aColumns[aCols[j]].sHeader2);
      PrintHdr(pRep,0," ");
    }
    PrintHdr(pRep,0,"\n");
    PrintHdr(pRep,1,"");
    for (j=0;j<iNumCols;j++) {      
      PrintHdr(pRep,0,aColumns[aCols[j]].sHeader3);
      PrintHdr(pRep,0," ");
    }
    PrintHdr(pRep,0,"\n");
    PrintHdr(pRep,1,"\n");
  }

}

void PrintHeadersOut() {
  PrintColHeaders(pRepOut,iNumOutputColumns,aOutputColumns);
}

void PrintHeadersRTD() {
  PrintColHeaders(pRepRTD,iNumRTDColumns,aRTDColumns);
}

void UpdateColumnsRow() {

  unsigned int j;
  int *aInt;
  FLOAT *aFloat;

  for (j=0;j<listColumns.iNumItems;j++) {
    if (aColumns[j].bActive) {
      switch(aColumns[j].eType)
      {
      case ColInt:
        aInt = (int *) aColumns[j].pColumnData;
        aInt[iRun] = *(int *)aColumns[j].pCurValue;
        break;
      case ColFloat:
        aFloat = (FLOAT *)aColumns[j].pColumnData;
        aFloat[iRun] = *(FLOAT *)aColumns[j].pCurValue;
        break;
      }
    }
  }
}

void PrintRow(REPORT *pRep, unsigned int iRow, unsigned int iNumCols, unsigned int *aCols) {

  unsigned int j;
  unsigned int *aInt;
  FLOAT *aFloat;

  REPORTCOL *pCol;

  Print(pRep,"  ");

  for (j=0;j<iNumCols;j++) {
    pCol = &aColumns[aCols[j]];
    switch(pCol->eType)
    {
    case ColInt:
      aInt = (unsigned int *) pCol->pColumnData;
      PrintUInt(pRep,pCol->sPrintFormat,aInt[iRow]);
      break;
    case ColFloat:
      aFloat = (FLOAT *)pCol->pColumnData;
      PrintFloat(pRep,pCol->sPrintFormat,aFloat[iRow]);
      break;
    }
    Print(pRep," ");
  }
  Print(pRep,"\n");

}

void PrintOutRow(unsigned int iRow) {
  PrintRow(pRepOut,iRow,iNumOutputColumns,aOutputColumns);
}

void PrintCurRow() {
  PrintRow(pRepOut,iRun,iNumOutputColumns,aOutputColumns);
}

void PrintRTDRow(unsigned int iRow) {
  PrintRow(pRepRTD,aSortedBySteps[iRow],iNumRTDColumns,aRTDColumns);
}


void PrintRTD() {
  unsigned int j;
  for (j=0;j<iNumFound;j++)
    PrintRTDRow(j);
}

void PrintTraceSolution() {
  unsigned int j;
  if (bSolutionFound) {
    PrintUInt(pRepState,"%d,",iRun);
    for (j=1;j<=iNumVar;j++) {
      if (aVarValue[j])
        Print(pRepSolution,"1");
      else
        Print(pRepSolution,"0");
    }
    Print(pRepSolution,"\n");
  }
}

void CreateFlipCounts() {
  aFlipCount = (unsigned int *)AllocateRAM((iNumVar+1)*sizeof(unsigned int));
}

void InitFlipCounts() {
  unsigned int j;
  for (j=0;j<=iNumVar;j++)
    aFlipCount[j] = 0;
}

void UpdateFlipCounts() {
  aFlipCount[iFlipCandidate]++;
}


void PrintTraceFlipCounts() {
  unsigned int j;
  PrintUInt(pRepFlipCounts,"%d,",iRun);
  PrintUInt(pRepFlipCounts,"%d",aFlipCount[0]);
  for (j=1;j<=iNumVar;j++) {
    PrintUInt(pRepFlipCounts,",%d",aFlipCount[j]);
  }
  Print(pRepFlipCounts,"\n");
}

void CalcPercentNullFlips() {
  if (iStep == 0) {
    fPercentNullFlips = 0.0f;
  } else {
    fPercentNullFlips = (FLOAT) (iNumNullFlips * 100);
    fPercentNullFlips /= (FLOAT) iStep;
  }
}

void CreateUnsatCounts() {
  aUnsatCount = (unsigned int *)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitUnsatCounts() {
  unsigned int j;
  for (j=0;j<iNumClause;j++)
    aUnsatCount[j] = 0;
}

void UpdateUnsatCounts() {
  unsigned int j;
  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0)
      aUnsatCount[j]++;
  }
}

void CreateClauseLast() {
  aClauseLast = (int*)AllocateRAM(iNumClause*sizeof(unsigned int));
}

void InitClauseLast() {
  unsigned int j;
  for (j=0;j<iNumClause;j++)
    aClauseLast[j] = -1;
}

void UpdateClauseLast() {
  unsigned int j;
  for (j=0;j<iNumClause;j++) {
    if (aNumTrueLit[j]==0)
      aClauseLast[j] = iStep;
  }
}



void PrintTraceUnsatCounts() {
  unsigned int j;
  PrintUInt(pRepUnsatCounts,"%d,",iRun);
  PrintUInt(pRepUnsatCounts,"%d",aUnsatCount[0]);
  for (j=1;j<iNumClause;j++) {
    PrintUInt(pRepUnsatCounts,",%d",aUnsatCount[j]);
  }
  Print(pRepUnsatCounts,"\n");
}


void PrintTraceVarLast() {
  unsigned int j;
  PrintUInt(pRepVarLast,"%d,",iRun);
  PrintUInt(pRepVarLast,"%d",aVarLastChange[1]);
  for (j=2;j<iNumVar;j++) {
    PrintUInt(pRepVarLast,",%d",aVarLastChange[j]);
  }
  Print(pRepVarLast,"\n");
}


void PrintTraceClauseLast() {
  unsigned int j;
  PrintUInt(pRepClauseLast,"%d,",iRun);
  PrintUInt(pRepClauseLast,"%d",aClauseLast[0]);
  for (j=1;j<iNumClause;j++) {
    PrintUInt(pRepClauseLast,",%d",aClauseLast[j]);
  }
  Print(pRepClauseLast,"\n");
}







void PrintTraceOptClauses() {
  unsigned int j;
  if (bSolutionFound) {
    PrintUInt(pRepOptClauses,"%d,",iRun);
    for (j=0;j<iNumClause;j++) {
      if (aNumTrueLit[j]==0)
        Print(pRepOptClauses,"0");
      else
        Print(pRepOptClauses,"1");
    }
    Print(pRepOptClauses,"\n");
  }
}

void PrintTraceState() {
  unsigned int j;
  if ((iRun==0)&&(iStep==1)) {
    PrintHdr(pRepState,1," Run ID, Step No., Num False, VarFlip, IsLocalMin, vararray\n");
  }
  PrintUInt(pRepState,"%d,",iRun);
  PrintUInt(pRepState,"%d,",iStep);
  PrintUInt(pRepState,"%d,",iNumFalse);
  PrintUInt(pRepState,"%d,",iFlipCandidate);
  PrintUInt(pRepState,"%d,",IsLocalMinimum());
  for (j=1;j<=iNumVar;j++) {
    if (aVarValue[j])
      Print(pRepState,"1");
    else
      Print(pRepState,"0");
  }
  Print(pRepState,"\n");
}

void InitSolveMode() {
  if (bSolveMode) {
    if (iFind==0)
      iFind = 1;
  }
}

void PrintSolutionModel() {
  unsigned int j;
  if ((bSolveMode)||(pRepModel->bActive)) {
    if (bSolutionFound) {
      PrintHdr(pRepModel,1,"\n");
      PrintHdrUInt(pRepModel,1,"Solution found for -target %d\n\n", iTarget);
      for (j=1;j<=iNumVar;j++) {
        if (!aVarValue[j]) {
          PrintUInt(pRepModel," -%d",j);
        } else {
          PrintUInt(pRepModel," %d",j);
        }
	      if (j % 10 == 0)
          Print(pRepModel,"\n");
      }
      if ((j-1) % 10 != 0) 
        Print(pRepModel,"\n");
    } else {
      PrintHdrUInt(pRepModel,1,"No Solution found for -target %d\n", iTarget);
    }
  }
}


void PrintSATCompetition() {
  unsigned int j;

  SetString(&sCommentString,"c");

  PrintUBCSATHeader(pRepSATComp);
  PrintAlgParmSettings(pRepSATComp,&parmUBCSAT);
  PrintHdr(pRepSATComp,1,"\n");
  PrintAlgParmSettings(pRepSATComp,&pAlg->parmList);
  PrintHdr(pRepSATComp,1,"\n");

  if (bSolutionFound) {
    Print(pRepSATComp,"s SATISFIABLE\n");
    Print(pRepSATComp,"v ");
    for (j=1;j<=iNumVar;j++) {
      if (!aVarValue[j]) {
        PrintUInt(pRepSATComp," -%d",j);
      } else {
        PrintUInt(pRepSATComp," %d",j);
      }
	    if (j % 10 == 0)
        Print(pRepSATComp,"\nv ");
    }
    Print(pRepSATComp," 0\n");
    /*    exit(10);*/
  }
  
    /*  } else {
    Print(pRepSATComp,"s UNKNOWN\n");
        exit(0);
	}
    */
}



void CalcPercentSolve() {
  fPercentSuccess = 100.0 * (FLOAT) iNumFound / (FLOAT) iRun;
}

void CalcFPS() {
  unsigned int j;
  REPORTCOL *pCol;

  pCol = &aColumns[FindItem(&listColumns,"steps")];
  aStepArray = (unsigned int *)pCol->pColumnData;
  fFlipsPerSecond = 0.0;
  for (j=0;j< iRun;j++) {
    fFlipsPerSecond += (FLOAT) aStepArray[j];
  }
  fFlipsPerSecond /= fTotalTime;
}


void PrintCNFStats() {

  unsigned int j;
  unsigned int iMaxClauseLen;
  FLOAT fAvgLitClause = 0.0;
  FLOAT fAvgVarOccur = 0.0;
  FLOAT fStdDevVarOccur = 0.0;
  FLOAT fTemp;
  unsigned int iNumPos;
  unsigned int iNumNeg;
  FLOAT fPosNegRatio;

  unsigned int *aClauseBins=0;
  

  PrintUInt(pRepCNFStats,"Clauses = %d\n",iNumClause);
  PrintUInt(pRepCNFStats,"Variables = %d \n",iNumVar);
  PrintUInt(pRepCNFStats,"TotalLiterals = %d\n",iNumLit);

  iMaxClauseLen = 0;
  for (j=0;j<iNumClause;j++) {
    if (aClauseLen[j] > iMaxClauseLen)
      iMaxClauseLen = aClauseLen[j];
  }
  PrintUInt(pRepCNFStats,"MaxClauseLen = %d\n",iMaxClauseLen);

  if (iNumClause > 0) {

    aClauseBins = (unsigned int *)AllocateRAM((iMaxClauseLen + 3) * sizeof(unsigned int));
    
    for (j=0;j<iMaxClauseLen + 3;j++) {
      aClauseBins[j] = 0;
    }

    for (j=0;j<iNumClause;j++) {
      aClauseBins[aClauseLen[j]]++;
    }

    PrintUInt(pRepCNFStats,"NumClauseLen_1 = %d \n",aClauseBins[1]);
    PrintUInt(pRepCNFStats,"NumClauseLen2 =  %d \n",aClauseBins[2]);
    PrintUInt(pRepCNFStats,"NumClauseLen3+ = %d \n",iNumClause - aClauseBins[1] - aClauseBins[2]);

    Print(pRepCNFStats,"FullClauseDistribution = ");
    for (j=0;j<=iMaxClauseLen;j++) {
      if (aClauseBins[j] > 0) {
        PrintUInt(pRepCNFStats," %d",j);
        PrintUInt(pRepCNFStats,":%d",aClauseBins[j]);
      }
    }
    Print(pRepCNFStats,"\n");

    fAvgLitClause = (FLOAT)iNumLit / (FLOAT) iNumClause;
    PrintFloat(pRepCNFStats,"MeanClauseLen = %f \n",fAvgLitClause);
    
    fAvgVarOccur = (FLOAT) iNumLit / (FLOAT) iNumVar;
    PrintFloat(pRepCNFStats,"MeanVariableOcc = %f \n",fAvgVarOccur);

    fStdDevVarOccur = 0.0; 
    for (j=1;j<=iNumVar;j++) {
      fTemp = (aNumLitOcc[GetPosLit(j)] + aNumLitOcc[GetNegLit(j)])-fAvgVarOccur;
      fStdDevVarOccur += (fTemp*fTemp);
    }
    fStdDevVarOccur /= (FLOAT) (iNumVar-1);
    fStdDevVarOccur = sqrt(fStdDevVarOccur);
    
    PrintFloat(pRepCNFStats,"StdDevVariableOcc = %f \n",fStdDevVarOccur);

    iNumPos = 0;
    iNumNeg = 0;
    for (j=1;j<=iNumVar;j++) {
      iNumPos += aNumLitOcc[GetPosLit(j)];
      iNumNeg += aNumLitOcc[GetNegLit(j)];
    }
    fPosNegRatio = (FLOAT) iNumPos / (FLOAT) iNumNeg;
    PrintUInt(pRepCNFStats,"NumPosLit = %d \n",iNumPos);
    PrintUInt(pRepCNFStats,"NumNegLit = %d \n",iNumNeg);
    PrintFloat(pRepCNFStats,"RatioPos:NegLit = %f \n",fPosNegRatio);
  }
}

