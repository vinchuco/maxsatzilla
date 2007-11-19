#ifndef FORWARDSELECTION_HH
#define FORWARDSELECTION_HH

#include <gsl/gsl_matrix.h>

#include "dataset.hh"

// If FSDEBUG is 1, output regarding Forward Selection computations will be shown
#define FSDEBUG 1

// This class implements the forward selection algorithm and works on a 
// dataset with multiple inputs and multiple outputs.
class ForwardSelection {
public:
  ForwardSelection(const MSZDataSet &, int);
  ~ForwardSelection();

  
  
private:
  gsl_matrix *fmatrix;///< Feature Matrix
  gsl_vector *ovec;   ///< Output Vector
};

#endif // FORWARDSELECTION_HH
