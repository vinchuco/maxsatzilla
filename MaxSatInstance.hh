
#include <fstream>
#include <vector>

#define MAX_LINE_LENGTH 1024
#define MAX_NUM_LITERALS 100

using namespace std;

class MaxSatInstance {
  string filename;
  enum {CNF, PARTIAL, WEIGHTED, WEIGHTED_PARTIAL} format;
  int numVars, numClauses;
  int *clauseLengths;
  vector<int> *negClausesWithVar, *posClausesWithVar;
  int unitClauses, binaryClauses, ternaryClauses;
  bool isTautologicalClause(int[MAX_NUM_LITERALS], int&, const int);
 public:
  MaxSatInstance( const char* filename );
  ~MaxSatInstance();

  void computeLocalSearchProperties();
  void printInfo( ostream& os );
};
