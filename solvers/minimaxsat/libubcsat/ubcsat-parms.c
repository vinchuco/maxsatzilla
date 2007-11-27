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

unsigned int bValidArgument[MAXTOTALPARMS];

unsigned int iNumTotalParms;
char **aTotalParms;

char sParmLine[MAXPARMLINELEN];
char sStringParm[MAXPARMLINELEN];

ALGORITHM *GetAlgorithm() {
  unsigned int j;
  if (*sAlgName) {
    for (j=0;j<iNumAlg;j++) {
      if (strcmp(sAlgName,aAlgorithms[j].sName)==0) {
        if (strcmp(sVarName,aAlgorithms[j].sVariant)==0) {
          if (aAlgorithms[j].bWeighted == bWeighted) {
            return(&aAlgorithms[j]);
          }
        }
      }
    }
  }
  return(0);
}

void CheckInvalidParamters() {

  unsigned int j;

  for (j=0;j<iNumTotalParms;j++) {
    if (!bValidArgument[j]) {
      HelpBadParm(aTotalParms[j]);
    }
  }
}

void SetDefaultParms(ALGPARMLIST *pParmList) {
  unsigned int j;
  ALGPARM *pParm;

  for (j=0;j<pParmList->iNumParms;j++) {
    pParm = &pParmList->aParms[j];

    switch (pParm->eType)
    {
      case TypeInt:
        *((unsigned int *)pParm->pParm1) = pParm->defDefault1.iInt;
        break;
      case TypeSInt:
        *((signed int *)pParm->pParm1) = pParm->defDefault1.iSInt;
	      break;
      case TypeBool:
        *((unsigned int *)pParm->pParm1) = pParm->defDefault1.bBool;
        break;
      case TypeRatio:
        *((unsigned int *)pParm->pParm1) = pParm->defDefault1.iInt;
        *((unsigned int *)pParm->pParm2) = pParm->defDefault2.iInt;
        break;
      case TypeString:
        SetString((char **) pParm->pParm1,pParm->defDefault1.sString);
        break;
      case TypeFloat:
        *((FLOAT *)pParm->pParm1) = pParm->defDefault1.fFloat;
        break;
      case TypeReport:
        break;
    }
  }
}

unsigned int MatchParameter(char *sSwitch,char *sParm) {
  
  char *pPos;
  char *pEndParm;
  
  pPos = strstr(sSwitch,sParm);
  while (pPos) {
    pEndParm = pPos + strlen(sParm);

    if ((*pEndParm==0)||(*pEndParm=='|')) {
      if (pPos==sSwitch) {
        return(1);
      } else {
        if (*(pPos-1)=='|')
          return(1);
      }
    }
    
    pPos = strstr(pPos + 1,sParm);
  }
  return(0);
}



void ParseParameters(ALGPARMLIST *pParmList) {

  unsigned int j;

  unsigned int iCurParm = 0;
  signed int iCurReport;
  REPORT *pRep;
  unsigned int iNumRepParms;
  ALGPARM *pParm;

  float fTemp;

  SetDefaultParms(pParmList);

  while (iCurParm < iNumTotalParms) {
    pParm = 0;
    for (j=0;j<pParmList->iNumParms;j++) {
      
      if (MatchParameter(pParmList->aParms[j].sSwitch,aTotalParms[iCurParm])) {
        pParm = &pParmList->aParms[j];
        bValidArgument[iCurParm++] = 1;
        break;
      }
    }

    if (pParm) {
      switch (pParm->eType)
      {
        case TypeInt:
        case TypeSInt:
          if (iCurParm == iNumTotalParms) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          if (sscanf(aTotalParms[iCurParm],"%u",(unsigned int*)pParm->pParm1)==0) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          bValidArgument[iCurParm++] = 1;
          break;
        case TypeBool:
          *((unsigned int *)pParm->pParm1) = 1;
          if (iCurParm < iNumTotalParms) {
            if (sscanf(aTotalParms[iCurParm],"%i",(int*)pParm->pParm1)) {
              bValidArgument[iCurParm++] = 1;
            }
          }
          break;
        case TypeRatio:
          if (iCurParm == iNumTotalParms) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          if (sscanf(aTotalParms[iCurParm],"%i",(int *)pParm->pParm1)==0) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          bValidArgument[iCurParm++] = 1;
          if (iCurParm < iNumTotalParms) {
            if (sscanf(aTotalParms[iCurParm],"%i",(int *)pParm->pParm2)) {
              bValidArgument[iCurParm++] = 1;
              if (*(unsigned int *)pParm->pParm2 == 0) {
                Print(pRepErr,"The 2nd parameter to a ratio can not be 0\n\n");
                HelpBadParm(aTotalParms[iCurParm-3]);
                AbnormalExit();
              }
            }
          }
          break;
        case TypeString:
          if (iCurParm == iNumTotalParms) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          SetString((char **) pParm->pParm1,aTotalParms[iCurParm]);
          bValidArgument[iCurParm++] = 1;
          break;
        case TypeFloat:
          if (iCurParm == iNumTotalParms) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          if (sscanf(aTotalParms[iCurParm],"%f",&fTemp)==0) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          *((FLOAT *)pParm->pParm1) = (FLOAT) fTemp;
          bValidArgument[iCurParm++] = 1;
          break;
        case TypeReport:
          if (iCurParm == iNumTotalParms) {
            HelpBadParm(aTotalParms[iCurParm-1]);
          }
          iCurReport = -1;          
          for (j=0;j<iNumReports;j++) {
            if (strcmp(aTotalParms[iCurParm],aReports[j].sID)==0) {
              iCurReport = j;
              break;
            }
          }
          if (iCurReport==-1) {
             HelpBadParm(aTotalParms[iCurParm-1]);
          }
          pRep = &aReports[iCurReport];
          
          pRep->bActive = 1;
          bValidArgument[iCurParm++] = 1;

          if (iCurParm < iNumTotalParms) {
            if (*aTotalParms[iCurParm] != '-') {
              SetString(&pRep->sOutputFile,aTotalParms[iCurParm]);
              bValidArgument[iCurParm++] = 1;
            }
            iNumRepParms = 0;
            while (iNumRepParms < pRep->iNumParms) {
              if (iCurParm < iNumTotalParms) {
                if (*aTotalParms[iCurParm] != '-') {
                  switch (pRep->aParmTypes[iNumRepParms])
                  {
                    case TypeInt:
                      if (sscanf(aTotalParms[iCurParm],"%i",(int *)pRep->aParameters[iNumRepParms])==0) {
                        HelpBadParm(aTotalParms[iCurParm]);
                      }
                      bValidArgument[iCurParm++] = 1;
                      break;
                    case TypeString:
                      SetString((char **) &pRep->aParameters[iNumRepParms],aTotalParms[iCurParm]);
                      bValidArgument[iCurParm++] = 1;
                      break;
		  default:
		    break;
                  }
                }
              }
              iNumRepParms++;
            }
          }
          break;      
      }
      
      pParm->bSpecified = 1;

      ParseItemList(&listFunctions,pParm->sFunctions,ActivateFunction);
      
    } else {
      iCurParm++;
    }
  }
}

ALGPARM *FindParm(ALGPARMLIST *pParmList, char *sSwitch) {

  unsigned int j;

  for (j=0;j<pParmList->iNumParms;j++) {
    if (MatchParameter(sSwitch,pParmList->aParms[j].sSwitch))
      return(&pParmList->aParms[j]);
  }
  return(0);
}


ALGPARM *AddParmCommon(ALGPARMLIST *pParmList,
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions)
{
  ALGPARM *p;
  
  if (pParmList->iNumParms >= MAXALGPARMS) {
    PrintUInt(pRepErr,"Unexpected Error: increase constant MAXALGPARMS [%d]\n",MAXALGPARMS);
    AbnormalExit();
  }

  p = &pParmList->aParms[pParmList->iNumParms++];

  SetString(&p->sSwitch,sSwitch);
  SetString(&p->sName,sName);
  SetString(&p->sDescription,sDescription);
  SetString(&p->sFunctions,sFunctions);
  p->bSpecified = 0;
  return(p);
}

void AddParmRatio(ALGPARMLIST *pParmList,
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pNum,
                  unsigned int *pDen,
                  unsigned int iDefNum,
                  unsigned int iDefDen)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pNum;
  p->pParm2 = (void *) pDen;
  p->defDefault1.iInt = iDefNum;
  p->defDefault2.iInt  = iDefDen;
  p->eType = TypeRatio;
}

void AddParmInt(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pInt,
                  unsigned int iDefInt)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pInt;
  p->defDefault1.iInt = iDefInt;
  p->eType = TypeInt;
}


void AddParmSInt(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  signed int *pSInt,
                  signed int iDefSInt)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pSInt;
  p->defDefault1.iInt = iDefSInt;
  p->eType = TypeSInt;
}

void AddParmBool(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  unsigned int *pBool,
                  unsigned int bDefBool)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pBool;
  p->defDefault1.bBool = bDefBool;
  p->eType = TypeBool;
}


void AddParmFloat(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  FLOAT *pFloat,
                  FLOAT fDefFloat)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pFloat;
  p->defDefault1.fFloat = fDefFloat;
  p->eType = TypeFloat;
}

void AddParmString(ALGPARMLIST *pParmList, 
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions,
                  char **pString,
                  char *sDefString)
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->pParm1 = (void *) pString;
  SetString(&p->defDefault1.sString,sDefString);
  p->eType = TypeString;
}

void AddParmReport(ALGPARMLIST *pParmList,
                  const char *sSwitch, 
                  const char *sName, 
                  const char *sDescription,
                  const char *sFunctions
                  )
{
  ALGPARM *p;
  p = AddParmCommon(pParmList,sSwitch,sName,sDescription,sFunctions);
  p->eType = TypeReport;
}
                      


void PrintAlgParmSettings(REPORT *pRep, ALGPARMLIST *pParmList) {
  
  unsigned int j;
  ALGPARM *pCurParm;

  for (j=0;j<pParmList->iNumParms;j++) {
    pCurParm = &pParmList->aParms[j];
    PrintHdrString(pRep,1," %s ",pCurParm->sSwitch);
    switch(pCurParm->eType)
    {
      case TypeInt:
        PrintHdrUInt(pRep,0,"%d ", *(unsigned int *)pCurParm->pParm1);
        break;
      case TypeSInt:
        PrintHdrSInt(pRep,0,"%d ", *(int *)pCurParm->pParm1);
        break;
      case TypeRatio:
        PrintHdrUInt(pRep,0,"%d ", *(unsigned int *)pCurParm->pParm1);
        PrintHdrUInt(pRep,0,"%d ", *(unsigned int *)pCurParm->pParm2);
        break;
      case TypeString:
        if (**(char **)pCurParm->pParm1 ==0) {
          PrintHdrString(pRep,0,"%s ","[null]");
        } else {
          PrintHdrString(pRep,0,"%s ", *(char **)pCurParm->pParm1);
        }
        break;
      case TypeFloat:
        PrintHdrFloat(pRep,0,"%f ", *(FLOAT *)pCurParm->pParm1);
        break;
      case TypeBool:
        PrintHdrUInt(pRep,0,"%d ", *(unsigned int *)pCurParm->pParm1);
        break;
      case TypeReport:
        break;
    }  
    PrintHdr(pRep,0,"\n");
  }
}

void PrintAlgSettings() {
  PrintHdrString(pRepOut,1," -alg %s",pAlg->sName);
  if (*(pAlg->sVariant))
    PrintHdrString(pRepOut,0," -v %s",pAlg->sVariant);
  if (pAlg->bWeighted)
    PrintHdr(pRepOut,0," -w");
  
  PrintHdr(pRepOut,0,"\n");

  PrintAlgParmSettings(pRepOut,&parmUBCSAT);
  PrintHdr(pRepOut,1,"\n");
  PrintAlgParmSettings(pRepOut,&pAlg->parmList);
  PrintHdr(pRepOut,1,"\n");  
}



void CheckParamterFile(int iCommandLineCount,char **aCommandLineArgs) {
  int j;
  char *sParameterFilename = 0;
  FILE *filParm;
  char *pStart;
  char *pPos;

  int iNumParmFiles = 0;

  for (j=0;j<(iCommandLineCount - 1);j++) {
    if (MatchParameter("-param|-fp",aCommandLineArgs[j])) {
      iNumParmFiles++;
    }
  }

  if (iNumParmFiles==0) {
    iNumTotalParms = iCommandLineCount - 1;
    if (iNumTotalParms==MAXTOTALPARMS) {
      PrintUInt(pRepErr,"Unexpected Error: increase constant MAXTOTALPARMS [%d]\n",MAXTOTALPARMS);
      AbnormalExit();
    }
    aTotalParms = &aCommandLineArgs[1];
    return;
  }

  aTotalParms =(char**) AllocateRAM(MAXTOTALPARMS * sizeof(char *));

  for (j=0;j<(iCommandLineCount - 1);j++) {
    if (MatchParameter("-param|-fp",aCommandLineArgs[j])) {
      
      sParameterFilename = aCommandLineArgs[j+1];

      SetupFile(&filParm,"r",sParameterFilename,stdin,0);

      while (!feof(filParm)) {
        if (fgets(sParmLine,MAXPARMLINELEN,filParm)) {
          if (strlen(sParmLine)==MAXPARMLINELEN-1) {
            PrintUInt(pRepErr,"Unexpected Error: increase constant MAXPARMLINELEN [%d]\n",MAXPARMLINELEN);
            AbnormalExit();
          }
          if ((*sParmLine)&&(*sParmLine != '#')) {
            pStart = sParmLine;
            pPos = strchr(pStart,' ');
            while (pPos) {
              if (pPos==pStart) {
                pStart++;
              } else {
                *pPos++=0;
                SetString(&aTotalParms[iNumTotalParms++],pStart);
                if (iNumTotalParms==MAXTOTALPARMS) {
                  PrintUInt(pRepErr,"Unexpected Error: increase constant MAXTOTALPARMS [%d]\n",MAXTOTALPARMS);
                  AbnormalExit();
                }
                pStart = pPos;
              }
              pPos = strchr(pStart,' ');
            }
            pPos = strchr(pStart,10);
            if (pPos) 
              *pPos = 0;
            pPos = strchr(pStart,13);
            if (pPos) 
              *pPos = 0;
            if (strlen(pStart)) {
              SetString(&aTotalParms[iNumTotalParms++],pStart);
              if (iNumTotalParms==MAXTOTALPARMS) {
                PrintUInt(pRepErr,"Unexpected Error: increase constant MAXTOTALPARMS [%d]\n",MAXTOTALPARMS);
                AbnormalExit();
              }
            }
          }
        }
      }

      CloseSingleFile(filParm);

    }
  }

  

  for (j=1;j<(iCommandLineCount);j++) {
    SetString(&aTotalParms[iNumTotalParms++],aCommandLineArgs[j]);;
    if (iNumTotalParms==MAXTOTALPARMS) {
      PrintUInt(pRepErr,"Unexpected Error: increase constant MAXTOTALPARMS [%d]\n",MAXTOTALPARMS);
      AbnormalExit();
    }
  }

}

}
