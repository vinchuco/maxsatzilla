#ifndef FORWARDSELECTION_HH
#define FORWARDSELECTION_HH

#include <vector>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "dataset.hh"

using std::vector;

// If FSDEBUG is 1, output regarding Forward Selection computations will be shown
#define FSDEBUG 1

// This class implements the forward selection algorithm and works on a 
// dataset with multiple inputs and multiple outputs.
class ForwardSelection {
public:
  ForwardSelection(const MSZDataSet &, size_t);
  ~ForwardSelection();

  /// Given a double which is the fin target to add variables,
  /// returns a vector with the indices of the features
  /// which should be included in the model.
  vector<int> run(double);
  
private:
  gsl_matrix *fmatrix;///< Feature Matrix
  gsl_vector *ovec;   ///< Output Vector
  size_t initVar;     ///< Initial FS Variable (highest correlated with output)
};

#endif // FORWARDSELECTION_HH
