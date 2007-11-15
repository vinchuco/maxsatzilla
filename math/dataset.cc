#include <iostream>

#include <cstdlib>
#include <cassert>

#include "dataset.hh"

using std::cerr;

MSZDataSet::MSZDataSet(double **matrix, int nrows, int ncols, int outputs) 
  : matrix(matrix), nrows(nrows), ncols(ncols), outputs(outputs) {}
  
MSZDataSet::~MSZDataSet() {

  // Please God, do not let this Seg Fault!

  // Clean up matrix by deleting each line.
  for(int row = 0; row < nrows; row++) 
    free(matrix[row]);
	 
  free(matrix);
  
}

/////////////////////////////////////////
/////////////////////////////////////////
//
// API Entrace Function for Data Set creation
//
/////////////////////////////////////////
/////////////////////////////////////////

MSZDataSet *createDataSet(double** matrix, int nrows, int ncols, int outputs) {
  assert(nrows > 0);
  assert(ncols > 0);
  assert(matrix != 0);
  assert(outputs > 0);
  
#ifndef NDEBUG
  for(int r = 0; r < nrows; r++)
    assert(matrix[r] != 0);
#endif // NDEBUG

  // The call!
  MSZDataSet *ds = new MSZDataSet(matrix, nrows, ncols, outputs);
  if(!ds) {
    cerr << "Error: Allocation of Dataset\n";
    exit(EXIT_FAILURE);
  }
  
  return ds;
}
