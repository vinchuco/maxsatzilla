
#include <fstream>
#include <vector>

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_LITERALS 100

using namespace std;

class MaxSatInstance {
  enum {CNF, PARTIAL, WEIGHTED, WEIGHTED_PARTIAL} format;
  int numVars, numClauses;
  int *clauseLengths;
  vector<int> *negClausesWithVar, *posClausesWithVar;
  int unitClauses, binaryClauses, ternaryClauses;
  bool isTautologicalClause(int[MAX_NUM_LITERALS], int&, const int);
 public:
  MaxSatInstance( const char* filename );
  ~MaxSatInstance();
  void printInfo( ostream& os );
};

bool MaxSatInstance::isTautologicalClause( int lits[ MAX_NUM_LITERALS ], int& numLits, const int clauseNum ) {
  // sort the clause and remove redundant literals
  //!! For large clauses better use sort()
  int temp, tempLit;
  for (int i=0; i<numLits-1; tempLit = lits[i++]) {
    for (int j=i+1; j<numLits; j++) {
      if (abs(tempLit) > abs(lits[j])) {
	temp = lits[j];
	lits[j] = tempLit;
	tempLit = temp;
      } else if (tempLit == lits[j]) {
	lits[j--] = lits[--numLits];
	printf("c literal %d is redundant in clause %d\n", tempLit, clauseNum);
      } else if (abs(tempLit) == abs(lits[j])) {
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

  while (infile.get() != 'p') {
    infile.ignore(MAX_LINE_LENGTH, '\n');
  }

  char strbuf[MAX_LINE_LENGTH];
  infile >> strbuf;

  if( strcmp(strbuf, "cnf")==0 ) {
    format = CNF;
  } else if( strcmp(strbuf, "wcnf")==0 ) {
    format = WEIGHTED;
  } else if( strcmp(strbuf, "pcnf")==0 ) {
    format = PARTIAL;
  } else if( strcmp(strbuf, "wpcnf")==0 ) {
    format = WEIGHTED_PARTIAL;
  } else {
    fprintf(stderr, "c Error: Can only understand cnf format!\n");
    exit(1);
  }

  infile >> numVars >> numClauses;

  clauseLengths = new int[ numClauses ];
  negClausesWithVar = new vector<int>[ numVars+1 ];
  posClausesWithVar = new vector<int>[ numVars+1 ];
  unitClauses = binaryClauses = ternaryClauses = 0;

  int lits[ MAX_NUM_LITERALS ];
 for (int clauseNum=0; clauseNum<numClauses; clauseNum++) {

    int numLits = 0;

    infile >> lits[numLits];
    while (lits[numLits] != 0)
      infile >> lits[++numLits];

    if ( numLits == 1 or !isTautologicalClause( lits, numLits, clauseNum )) {
      clauseLengths[clauseNum] = numLits;
      switch( numLits ) {
      case 1: unitClauses++; break;
      case 2: binaryClauses++; break;
      case 3: ternaryClauses++; break;
      }
      
      for (int litNum = 0; litNum < numLits; litNum++)
	if (lits[litNum] < 0)
	  negClausesWithVar[abs(lits[litNum])].push_back(clauseNum);
	else
	  posClausesWithVar[lits[litNum]].push_back(clauseNum);
      
    } else {
      clauseNum--;
      numClauses--;
    }
  }
}

MaxSatInstance::~MaxSatInstance() {
  delete clauseLengths;
}

void MaxSatInstance::printInfo(ostream& os) {
  os << "Num vars: " << numVars << endl;
  os << "Num clauses: " <<  numClauses << endl;
  os << "Ratio c/v: " << float(numClauses/numVars) << endl;  
  for (int varNum=1; varNum<=numVars; varNum++) {
    os << "Neg [" << varNum << "]: " << negClausesWithVar[ varNum ].size() << endl;
    os << "Pos [" << varNum << "]: " << posClausesWithVar[ varNum ].size() << endl;
  }
  os << "Unary: " << unitClauses << endl;
  os << "Binary: " << binaryClauses << endl;
  os << "Ternary: " << ternaryClauses << endl;
}
