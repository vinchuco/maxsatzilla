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

#ifndef UBCSAT

#define UBCSAT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ubcsat-limits.h"
#include "ubcsat-types.h"

#include "ubcsat-alg.h"
#include "ubcsat-parms.h"
#include "ubcsat-lit.h"
#include "ubcsat-mem.h"
#include "ubcsat-cnf.h"
#include "ubcsat-time.h"
#include "ubcsat-reports.h"
#include "ubcsat-io.h"
#include "ubcsat-defaults.h"
#include "ubcsat-help.h"

#include "algorithms.h"
#include "reports.h"

#include "mylocal.h"

namespace ubcsat { int main(int,char**); }

#endif

