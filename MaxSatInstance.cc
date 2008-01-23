/*******************************************************************************[MaxSatInstance.cc]
Magpie -- Florian Letombe, Joao Marques-Silva, Paulo Matos, Jordi Planes - 2008

Parts of the code in this file have been extracted from SATzilla.
**************************************************************************************************/

#include <algorithm>

#include "MaxSatInstance.hh"
#include "ubcsat/ubcsat.h"

bool MaxSatInstance::isTautologicalClause( int lits[ MAX_NUM_LITERALS ], int& numLits, const int clauseNum ) {
  // sort the clause and remove redundant literals
  //!! For large clauses better use sort()
  for (int i=0; i<numLits-1; i++) {
    for (int j=i+1; j<numLits; j++) {
      if (abs(lits[i]) > abs(lits[j])) { // Bubble sort
        swap( lits[i], lits[j] );
      } else if (lits[i] == lits[j]) {
	lits[j--] = lits[--numLits];
	printf("c literal %d is redundant in clause %d\n", lits[i], clauseNum);
      } else if (abs(lits[i]) == abs(lits[j])) {
	printf("c Clause %d is tautological.\n", clauseNum);
	return true;
      }
    }
  }              
  return false;
}

MaxSatInstance::MaxSatInstance( const char* filename ) 
{
  ifstream infile(filename);
  if (!infile) {
    fprintf(stderr, "c Error: could not read from %s.\n", filename);
    exit(1);
  }
  inputFileName = new char[ strlen( filename ) + 1 ];
  strcpy( inputFileName, filename );

  while (infile.get() != 'p') {
    infile.ignore(MAX_LINE_LENGTH, '\n');
  }

  char strbuf[MAX_LINE_LENGTH];
  infile >> strbuf;

  if( strcmp(strbuf, "cnf")==0 ) {
    format = CNF;
  } else if( strcmp(strbuf, "wcnf")==0 ) {
    format = WEIGHTED;
  } else if( strcmp(strbuf, "pmcnf")==0 ) {
    format = PARTIAL;
  } else if( strcmp(strbuf, "wpmcnf")==0 ) {
    format = WEIGHTED_PARTIAL;
  } else {
    fprintf(stderr, "c Error: Format %s not recognized.\n", strbuf);
    exit(1);
  }

  infile >> numVars >> numClauses;

  negClausesWithVar = new int[ numVars+1 ];
  posClausesWithVar = new int[ numVars+1 ];
  fill( negClausesWithVar, &negClausesWithVar[numVars+1], 0 );
  fill( posClausesWithVar, &posClausesWithVar[numVars+1], 0 );

  sUnitClauses = sBinaryClauses = sTernaryClauses = 0;
  hUnitClauses = hBinaryClauses = hTernaryClauses = 0;
  hNumClauses = sNumClauses = 0;

  int lits[ MAX_NUM_LITERALS ];
  int numLits = 0;
  bool hardClauses = format == PARTIAL ? true : false;
  for (int clauseNum=0; clauseNum<numClauses; clauseNum++) {

    infile.get(); // newline
    if ( format == PARTIAL ) {      
      if ( infile.peek() == 'p' ) {
	infile >> strbuf;
	if ( strcmp( strbuf, "hard" ) == 0 ) printf("Hard\n");
	else if ( strcmp( strbuf, "soft" ) == 0 ) {
	  printf("Soft\n");
	  hardClauses = false;
	}
	infile.ignore( MAX_LINE_LENGTH, '\n' );
      }
    }

    numLits = 0;
    
    infile >> lits[numLits];
    while (lits[numLits] != 0) {
      infile >> lits[++numLits];
#ifndef NDEBUG
      if ( numLits >= MAX_NUM_LITERALS ) throw "literals = MAX_NUM_LITERALS";
#endif
    }

    if ( numLits == 1 or !isTautologicalClause( lits, numLits, clauseNum )) {
      if ( hardClauses ) {
	hNumClauses++;
	switch( numLits ) {
	case 1: hUnitClauses++; break;
	case 2: hBinaryClauses++; break;
	case 3: hTernaryClauses++; break;
	}
      } else {
	switch( numLits ) {
	case 1: sUnitClauses++; break;
	case 2: sBinaryClauses++; break;
	case 3: sTernaryClauses++; break;
	}
      }
 
      for (int litNum = 0; litNum < numLits; litNum++) 
	if (lits[litNum] < 0)
	  negClausesWithVar[abs(lits[litNum])]++;
	else
	  posClausesWithVar[lits[litNum]]++;
      
    } else {
      clauseNum--;
      numClauses--;
    }
  }
  sNumClauses = numClauses - hNumClauses;
}

MaxSatInstance::~MaxSatInstance() {
  delete negClausesWithVar;
  delete posClausesWithVar;
}

char* MaxSatInstance::getFileName() {
  char *newName;
  if ( format == PARTIAL ) {
    char command[ MAX_LINE_LENGTH ];
    newName = new char[ MAX_LINE_LENGTH ];
    sprintf(newName, "%s", P_tmpdir);
    strcat(newName, "/pmcnf2wcnfXXXXXX");
    newName = mktemp(newName);
    sprintf( command, "./scripts/pmcnf2wcnf %s > %s", inputFileName, newName );
    //printf( "Command : %s\n", command );
    system( command );
  } else {
    newName = new char[ strlen( inputFileName ) ];
  }
  return newName;
}

namespace ubcsat { int main(int, char**); }

void MaxSatInstance::computeLocalSearchProperties() {

  char sTimeout[64];
  char sRuns[64];
  char *fileName = getFileName();

  sprintf(sTimeout, "%d", UBCSAT_TIME_LIMIT);
  sprintf(sRuns, "%d", UBCSAT_NUM_RUNS);


  char* vlineFilename = new char[512];
  sprintf(vlineFilename, "%s", P_tmpdir);
  strcat(vlineFilename, "/XXXXXX");
  vlineFilename = mktemp(vlineFilename);


  char strseed[64];
  sprintf(strseed, "%d", UBCSAT_SEED );

  char* argv[] = {"ubcsat", 
		  "-alg", NULL, "-noimprove", NULL,  "-i", fileName, 
		  "-runs", sRuns, "-timeout", sTimeout,
		  "-r", "satcomp", vlineFilename,
		  "-seed", strseed,
		  "-satzilla", "-w" };
  
  int argc = 17;

  if ( format == PARTIAL ) {
    argc++;
  }
  
  // -- do saps
  argv[2]="novelty+";
  argv[4]="0.1";
  
  if ( ubcsat::main(argc, argv) == 10 ) printf("Instance satisfiable\n");
  
  // -- do gsat
  argv[2]="gwsat";
  argv[4]="0.5";

  if ( ubcsat::main(argc, argv) == 10 ) printf("Instance satisfiable\n");

  delete[] vlineFilename;
  delete[] fileName;
}

void MaxSatInstance::printInfo(ostream& os) {
  os << "Number of Variables: " << numVars << endl;
  os << "Number of Clauses: " <<  numClauses << endl;
  os << "Ratio Clauses/Variables: " << (float)numClauses/numVars << endl;
  if ( format == PARTIAL ) {
    os << "Number of Hard Clauses: " << hNumClauses << endl;
    os << "Ratio Hard Clauses/Total Clauses: " << (float)hNumClauses/numClauses << endl;
  }
  int negClauses = 0, posClauses = 0;
  for (int varNum=1; varNum<=numVars; varNum++) {
    negClauses += negClausesWithVar[ varNum ];
    posClauses += posClausesWithVar[ varNum ];
  }
  os << "Ratio Negative Clauses: " << (float)negClauses/numClauses << endl;
  os << "Ratio Positive Clauses: " << (float)posClauses/numClauses << endl;
  os << "Ratio Unit Clauses: " << (float)sUnitClauses/sNumClauses << endl;
  os << "Ratio Binary Clauses: " << (float)sBinaryClauses/sNumClauses << endl;
  os << "Ratio Ternary Clauses: " << (float)sTernaryClauses/sNumClauses << endl;
  if ( format == PARTIAL ) {
    os << "Ratio Hard Unit Clauses: " << (float)hUnitClauses/hNumClauses << endl;
    os << "Ratio Hard Binary Clauses: " << (float)hBinaryClauses/hNumClauses << endl;
    os << "Ratio Hard Ternary Clauses: " << (float)hTernaryClauses/hNumClauses << endl;
  }
}
