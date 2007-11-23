
#include <iostream>
#include "math/dataset.hh"
#include "math/forwardselection.hh"

#define INSTANCES 3
#define SOLVERS 1
#define FEATURES 2
#define TOTAL_COLUMNS (SOLVERS+FEATURES)

using namespace std;

int main( int argc, char* argv[] ) {
  //static char* labels[TOTAL_COLUMNS] = {"maxsatz", "Vars", "Clauses", "C/V", "NegativeLiterals", "PositiveLiterals", "UnitClauses", "BinaryClauses", "TernaryClauses"};
  static char* labels[TOTAL_COLUMNS] = {"maxsatz", "Vars", "Clauses" };
  double **data;

  data = new double*[INSTANCES];
  for( int i = 0; i < TOTAL_COLUMNS; i++)
    data[i] = new double[TOTAL_COLUMNS];

  //instance bf0432-043.cnf
  data[0][0] = 100;
  data[0][1] = 865;
  data[0][2] = 2783;
  //data[0][3] = 3.21734;
  //data[0][4] = 1.60115;
  //data[0][5] = 0.98383;
  //data[0][6] = 0.00215595;
  //data[0][7] = 0.5106;
  //data[0][8] = 0.401365;

  //instance bf1355-127.cnf
  data[1][0] = 150;
  data[1][1] = 2297;
  data[1][2] = 7306;
  //data[1][3] = 3.18067;
  //data[1][4] = 1.4792;
  //data[1][5] = 1.05995;
  //data[1][6] = 0.000684369;
  //data[1][7] = 0.538188;
  //data[1][8] = 0.392691;

  //instance C220_FV_SZ_65.cnf
  data[2][0] = 50;
  data[2][1] = 1728;
  data[2][2] = 4496;
  //data[2][3] = 2.60185;
  //data[2][4] = 1.73043;
  //data[2][5] = 0.403247;
  //data[2][6] = 0.262011;
  //data[2][7] = 0.58919;
  //data[2][8] = 0.0700623;

  MSZDataSet *ds = createDataSet( data, INSTANCES, TOTAL_COLUMNS, SOLVERS );

  ds->dumpPlotFiles( labels, TOTAL_COLUMNS, "./driver" );

  ForwardSelection fs( *ds, 0 );
  vector<size_t> columns_to_take = fs.run( 0.15 );

  return 0;
}
