
#include <iostream>
#include "MaxSatInstance.hh"
#include "math/dataset.hh"
// #include "math/forwardselection.hh"

#define INSTANCES 2
#define SOLVERS 1
#define FEATURES 8
#define TOTAL_COLUMNS (SOLVERS+FEATURES)

using namespace std;

int main( int argc, char* argv[] ) {
  static char* labels[TOTAL_COLUMNS] = {"maxsatz", "Vars", "Clauses", "C/V", "NegativeLiterals", "PositiveLiterals", "UnitClauses", "BinaryClauses", "TernaryClauses"};
  double **data;

  data = new double*[INSTANCES];
  for( int i = 0; i < TOTAL_COLUMNS; i++)
    data[i] = new double[TOTAL_COLUMNS];

  //instance bf0432-043.cnf
  data[0][0] = 100;
  data[0][1] = 865;
  data[0][2] = 2783;
  data[0][3] = 3.21734;
  data[0][4] = 1.60115;
  data[0][5] = 0.98383;
  data[0][6] = 0.00215595;
  data[0][7] = 0.5106;
  data[0][8] = 0.401365;

  //instance bf1355-127.cnf
  data[1][0] = 150;
  data[1][1] = 2297;
  data[1][2] = 7306;
  data[1][3] = 3.18067;
  data[1][4] = 1.4792;
  data[1][5] = 1.05995;
  data[1][6] = 0.000684369;
  data[1][7] = 0.538188;
  data[1][8] = 0.392691;

  MSZDataSet *ds = createDataSet( data, INSTANCES, TOTAL_COLUMNS, SOLVERS );

  ds->dumpPlotFiles( labels, TOTAL_COLUMNS, "./driver" );

  //  ForwardSelection fs( *ds, 0 );
  //  vector<int> res = fs.run( 0.15 );

  return 0;
}
