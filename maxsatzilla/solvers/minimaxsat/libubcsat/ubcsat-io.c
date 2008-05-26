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

void RandomSeed(unsigned int iSeed);
unsigned int RandomMax();
unsigned int RandomInt(unsigned int iMax);
unsigned int RandomRatio(unsigned int iNumerator, unsigned int iDenominator);
FLOAT RandomFloat();

FXNRANDSEED fxnRandomSeed = RandomSeed;
FXNRANDMAX fxnRandomMax = RandomMax;
FXNRANDINT fxnRandomInt = RandomInt;
FXNRANDRATIO fxnRandomRatio = RandomRatio;
FXNRANDFLOAT fxnRandomFloat = RandomFloat;

FXNRANDMAX fxnCountedRand;

#define MERSENNE

#ifdef MERSENNE

extern unsigned int genrand_int32();
extern void init_genrand(unsigned long s);

FXNRANDMAX fxnRandUInt32 = genrand_int32;
#define fxnRandSeed(A) init_genrand(A)

#else
#ifdef WIN32
FXNRANDMAX fxnRandUInt32 = (unsigned int (*)()) rand;
#define fxnRandSeed(A) srand(A)
#else
FXNRANDMAX fxnRandUInt32 = random;
#define fxnRandSeed(A) srandom(A)
#endif
#endif


FLOAT RandomFloat() {
  unsigned int iNum;
  FLOAT fNum;
  iNum = fxnRandUInt32();
  fNum = (FLOAT) iNum;
  fNum /= 0xFFFFFFFF;
  return(fNum);
}


unsigned int RandomInt(unsigned int iMax) {
  return(fxnRandUInt32() % iMax);
}

unsigned int RandomRatio(unsigned int iNumerator, unsigned int iDenominator) {
  if ((iNumerator==0)||(iDenominator==0)) {
    return(0) ;
  } else {
    if (fxnRandUInt32() % iDenominator < iNumerator) {
      return(1);
    } else {
      return(0);
    }
  } 
}

unsigned int RandomMax() {
  return(fxnRandUInt32());
}

void RandomSeed(unsigned int iSeed) {
  fxnRandSeed(iSeed);
}

void SetupFile(FILE **fFil,const char *sOpenType, const char *sFilename, FILE *filDefault, unsigned int bAllowNull) {

  if (*sFilename) {
    if (strcmp(sFilename,"null")==0) {
      if (bAllowNull) {
        (*fFil) = 0;  
      } else {
        fprintf(stderr,"Fatal Error: Invalid use of null i/o\n");
        AbnormalExit();
      }
    } else {
      (*fFil) = fopen(sFilename,sOpenType);
      if ((*fFil) == 0) {
        printf("Fatal Error: Invalid filename [%s] specified \n",sFilename);
        AbnormalExit();
      }
    }
  } else {
    (*fFil) = filDefault;
  }
}

void SetupReports() {

  unsigned int j;

  for (j=0;j<iNumReports;j++) {
    if (strcmp(aReports[j].sOutputFile,"null")==0) {
      /* aReports[j].bActive = 0;*/
      aReports[j].fileOut = 0;
    }
    if (aReports[j].bActive) {
      if (strcmp(aReports[j].sOutputFile,"stdout")==0) {
        aReports[j].fileOut = stdout;
      } else if (strcmp(aReports[j].sOutputFile,"stderr")==0) {
        aReports[j].fileOut = stderr;
      } else if (strcmp(aReports[j].sOutputFile,"null")!=0) {
        aReports[j].fileOut = fopen(aReports[j].sOutputFile,"w");
        if (aReports[j].fileOut == 0) {
          printf("Fatal Error: Invalid filename [%s] specified \n",aReports[j].sOutputFile);
          AbnormalExit();
        }
      }
      ParseItemList(&listFunctions,aReports[j].sFunctions,ActivateFunction);
    }
  }

}

void CloseSingleFile(FILE *filToClose) {
  if (filToClose) {
    if ((filToClose != stdout) && (filToClose != stderr)) {
      fclose(filToClose);
    }
  }
}

void CloseReports() {
  unsigned int j;
  for (j=0;j<iNumReports;j++) {
    if (aReports[j].bActive) {
      CloseSingleFile(aReports[j].fileOut);
    }
  }
}

void Print(REPORT *pRep, const char *sFormat) {
  FILE *filOut = 0;
  if (pRep)
    filOut = pRep->fileOut;
  if (filOut)
    fprintf(filOut,sFormat);
  if (bReportEcho && (filOut != stdout))
    fprintf(stdout,sFormat);
}

void PrintHdr(REPORT *pRep, unsigned int bCommentChar, const char *sFormat) {
  FILE *filOut = 0;

  if (bReportClean)
    return;
  if (pRep)
    filOut = pRep->fileOut;
  if (bCommentChar) {
    if (filOut) {
      fprintf(filOut,"%s ",sCommentString);
    }
    if (bReportEcho && (filOut != stdout)) {
      fprintf(stdout,"%s ",sCommentString);
    }
  }
  Print(pRep,sFormat);
}

void PrintUInt(REPORT *pRep, const char *sFormat, unsigned int iVal) {
  FILE *filOut = 0;
  if (pRep)
    filOut = pRep->fileOut;
  if (filOut)
    fprintf(filOut,sFormat,iVal);
  if (bReportEcho && (filOut != stdout))
    fprintf(stdout,sFormat,iVal);
}

void PrintHdrUInt(REPORT *pRep, unsigned int bCommentChar,const char *sFormat,unsigned int iVal) {
  FILE *filOut = 0;

  if (bReportClean)
    return;
  if (pRep)
    filOut = pRep->fileOut;
  if (bCommentChar) {
    if (filOut) {
      fprintf(filOut,"%s ",sCommentString);
    }
    if (bReportEcho && (filOut != stdout)) {
      fprintf(stdout,"%s ",sCommentString);
    }
  }
  PrintUInt(pRep,sFormat,iVal);
}


void PrintSInt(REPORT *pRep, const char *sFormat, signed int iVal) {
  FILE *filOut = 0;
  if (pRep)
    filOut = pRep->fileOut;  
  if (filOut)
    fprintf(filOut,sFormat,iVal);
  if (bReportEcho && (filOut != stdout))
    fprintf(stdout,sFormat,iVal);
}

void PrintHdrSInt(REPORT *pRep, unsigned int bCommentChar, const char *sFormat, signed int iVal) {
  FILE *filOut = 0;

  if (bReportClean)
    return;
  if (pRep)
    filOut = pRep->fileOut;
  if (bCommentChar) {
    if (filOut) {
      fprintf(filOut,"%s ",sCommentString);
    }
    if (bReportEcho && (filOut != stdout)) {
      fprintf(stdout,"%s ",sCommentString);
    }
  }
  PrintSInt(pRep,sFormat,iVal);
}


void PrintFloat(REPORT *pRep, const char *sFormat, FLOAT fVal) {
  FILE *filOut = 0;
  if (pRep)
    filOut = pRep->fileOut;
  if (filOut)
    fprintf(filOut,sFormat,fVal);
  if (bReportEcho && (filOut != stdout))
    fprintf(stdout,sFormat,fVal);
}


void PrintHdrFloat(REPORT *pRep, unsigned int bCommentChar, const char *sFormat, FLOAT fVal) {
  FILE *filOut = 0;

  if (bReportClean)
    return;
  if (pRep)
    filOut = pRep->fileOut;
  if (bCommentChar) {
    if (filOut) {
      fprintf(filOut,"%s ",sCommentString);
    }
    if (bReportEcho && (filOut != stdout)) {
      fprintf(stdout,"%s ",sCommentString);
    }
  }
  PrintFloat(pRep,sFormat,fVal);
}


void PrintString(REPORT *pRep, const char *sFormat, char *sString) {
  FILE *filOut = 0;
  if (pRep)
    filOut = pRep->fileOut;
  if (filOut)
    fprintf(filOut,sFormat,sString);
  if (bReportEcho && (filOut != stdout))
    fprintf(stdout,sFormat,sString);
}


void PrintHdrString(REPORT *pRep, unsigned int bCommentChar, const char *sFormat, char *sString) {
  FILE *filOut = 0;

  if (bReportClean)
    return;
  if (pRep)
    filOut = pRep->fileOut;
  if (bCommentChar) {
    if (filOut) {
      fprintf(filOut,"%s ",sCommentString);
    }
    if (bReportEcho && (filOut != stdout)) {
      fprintf(stdout,"%s ",sCommentString);
    }
  }
  PrintString(pRep,sFormat,sString);
}


void AbnormalExit() {
  CloseReports();  
  FreeRAM();
  exit(1);
}

void CleanExit() {
  CloseReports();
  FreeRAM();
  /*  exit(0);*/
}


unsigned int iNumRandomCalls;
FXNRANDMAX fxnRandOrig;

unsigned int CountRandom() {
  iNumRandomCalls++;
  return(fxnRandOrig());
}

void SetupCountRandom() {
  fxnRandOrig = fxnRandUInt32;
  fxnRandUInt32 = CountRandom;
}

void InitCountRandom() {
  iNumRandomCalls = 0;
}

}
