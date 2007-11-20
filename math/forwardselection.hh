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

  /// Given a double which is the fin target to add regressors,
  /// returns a vector with the indices of the regressors
  /// which should be included in the model.
  vector<size_t> run(double);
  
private:
  double ForwardSelection::computeFtest(const vector<bool> &, double, size_t, double *);

  gsl_matrix *fmatrix;///< Feature Matrix
  gsl_vector *ovec;   ///< Output Vector
  double SSt;     ///< Total Corrected Sum of Squares
  size_t initRegressor;     ///< Initial FS Regressor (highest correlated with output)
  double initSSr;     ///< Initial regression sum of squares 
};

#endif // FORWARDSELECTION_HH
