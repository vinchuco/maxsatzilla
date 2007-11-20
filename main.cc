
#include <iostream>
#include "MaxSatInstance.hh"
#include "math/dataset.hh"
#include "math/forwardselection.hh"

using namespace std;

int main( int argc, char* argv[] ) {
  if ( argc <= 1 ) exit(-1);

  MaxSatInstance msi( argv[1] );
  msi.printInfo( cout );

  static char* labels[5] = {"Solver1", "Solver2", "Vars", "Clauses", "SomethingElse"};
  double **data;
  data = new double*[3];
  for( int i = 0; i < 3; i++)
    data[i] = new double[5];

  MSZDataSet *ds = createDataSet( data, 3, 5, 2 );

  ds->dumpPlotFiles( labels, 5, "./driver" );

  ForwardSelection fs( *ds, 0 );

  vector<int> res = fs.run( 0.15 );

  return 0;
}
