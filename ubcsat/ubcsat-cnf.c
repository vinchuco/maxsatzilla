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
#include <cassert>

namespace ubcsat {

  unsigned int iNumVar = 0;
  unsigned int iVARSTATELen = 0;
  unsigned int iNumClause = 0;
  unsigned int iNumLit = 0;
  unsigned int *aClauseLen;
  FLOAT *aClauseWeight;
  FLOAT fTotalWeight;
  LITTYPE **pClauseLits;  

  char sLine[MAXCNFLINELEN];

  /* 
     ReadCNF()
     - Reads in the data structure of the SAT problem
     Currently supports .cnf and .wcnf files
     This routine could be made more robust

     - Groups all literals in sizes of LITSPERCHUNK
  */

  void ReadCNF() {
    unsigned int j = 0;
    unsigned int k = 0;
    unsigned int bIsWCNF = 0; 
    float fDummy = 0;
    signed int l = 0;
  

    LITTYPE *pData = 0;
    LITTYPE *pNextLit = 0;
    LITTYPE *pLastLit = 0;

    FILE *filInput = 0;

    bIsWCNF = 0;

    iNumClause = 0;

    SetupFile(&filInput,"r",sFilenameIn,stdin,0);

    while (iNumClause == 0) {
      fgets(sLine,MAXCNFLINELEN,filInput);
      if (strlen(sLine)==MAXCNFLINELEN-1) {
	PrintUInt(pRepErr,"Unexpected Error: increase constant MAXCNFLINELEN [%d]\n",MAXCNFLINELEN);
	AbnormalExit();
      }

      if (strncmp(sLine,"p wcnf",6)==0)
	bIsWCNF = 1;

      if (sLine[0] =='p') {
	if (bWeighted) {
	  if (bIsWCNF) {
	    sscanf(sLine,"p wcnf %d %d",&iNumVar,&iNumClause);
	  } else {
	    Print(pRepErr,"Warning! reading .cnf file and setting all weights = 1\n");
	    sscanf(sLine,"p cnf %d %d",&iNumVar,&iNumClause);
	  }
	} else {
	  if (bIsWCNF) {
	    Print(pRepErr,"Warning! reading .wcnf file and ignoring all weights\n");
	    sscanf(sLine,"p wcnf %d %d",&iNumVar,&iNumClause);
	  } else {
	    sscanf(sLine,"p cnf %d %d",&iNumVar,&iNumClause);
	  }
	}
      }
    }
  
    aClauseLen = (unsigned int*)AllocateRAM(iNumClause * sizeof(unsigned int));
    pClauseLits = (unsigned int**)AllocateRAM(iNumClause * sizeof(LITTYPE *));
    if (bWeighted)
      aClauseWeight = (double *)AllocateRAM(iNumClause * sizeof(FLOAT));
  
    pLastLit = pNextLit = pData = 0;

    iNumLit = 0;

    for (j=0;j<iNumClause;j++) {

      if (bWeighted) {
	if (bIsWCNF) {
	  fscanf(filInput,"%f",&fDummy);
	  aClauseWeight[j] = (FLOAT) fDummy;
	} else {
	  aClauseWeight[j] = 1.0f;
	}
	fTotalWeight += aClauseWeight[j];
      } else {
	if (bIsWCNF) {
	  fscanf(filInput,"%f",&fDummy);
	}
      }

      pClauseLits[j] = pNextLit;
      aClauseLen[j] = 0;

      if(fgetc(filInput) == 'c') {
	while(fgetc(filInput) != '\n');
	j--;
	continue;
      }

      do {
	fscanf(filInput,"%d",&l);
	if (l) {
        
	  if (pNextLit == pLastLit) {
	    pData = (unsigned int*)AllocateRAM(LITSPERCHUNK * sizeof(LITTYPE));
	    assert(pData != 0);
	    pNextLit = pData;
	    pLastLit = pData + LITSPERCHUNK;
	    for (k=0;k<aClauseLen[j];k++) {
	      *pNextLit = pClauseLits[j][k];
	      pNextLit++;
	    }
	    pClauseLits[j] = pData;
	  }
	  assert(pNextLit != 0);
	  *pNextLit = SetLitFromFile(l);
	  pNextLit++;
	  aClauseLen[j]++;
	  iNumLit++;
	}
      } while (l != 0);
    }

    AdjustLastRAM((pNextLit - pData) * sizeof(LITTYPE));

    iVARSTATELen = (iNumVar / 32) + 1;
    if ((iNumVar % 32)==0) iVARSTATELen--;

    /** THIS IS A HUGE HACK, BUT I'M NOT SURE WHERE ELSE TO PUT THIS **/
    iNoImprove=(int)((FLOAT)iNumVar * fNoImproveFactor);

    CloseSingleFile(filInput);  

  }

}
