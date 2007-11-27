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

/* 
  BuildAlgorithmList()
  - Central routine for adding algorithms to the algorithm table
*/


void BuildAlgorithmList() {

  memset(aNumFxns,0,sizeof(unsigned int) * NUMFXNTYPES);
  pRepErr->fileOut = stderr;
  
  AddDEFAULT(); /* this should be the first call */
  
  AddGSAT();  
  AddGWSAT();
  AddGSATTABU();

  AddHSAT();
  AddHWSAT();

  AddWALKSAT();
  AddWALKSATTABU();

  AddNOVELTY();
  AddNOVELTYPLUS();

  AddRNOVELTY();
  AddRNOVELTYPLUS();

  AddSAPS();

  AddWeightedAlgs();

}

