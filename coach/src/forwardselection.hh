#ifndef FORWARDSELECTION_HH
#define FORWARDSELECTION_HH

#include <vector>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include "dataset.hh"
#include "pputils.hh"

using std::vector;

// This class implements the forward selection algorithm and works on a 
// dataset with multiple inputs and multiple outputs.
class ForwardSelection {
public:
  ForwardSelection(const MSZDataSet &);
  ~ForwardSelection();

  /// Given a double which is the fin target to add regressors,
  /// returns a vector with the indices of the regressors
  /// which should be included in the model.
  vector<uint> run(double);
  vector<uint> runForBest(uint); ///< Returns a vector with the best n variables.
  
private:
  double computeFtest(const vector<bool> &, double, uint, double *);

  gsl_matrix *fmatrix;///< Feature Matrix
  gsl_vector *ovec;   ///< Output Vector
  double SSt;         ///< Total Corrected Sum of Squares
  uint initRegressor; ///< Initial FS Regressor (highest correlated with output)
  double initSSr;     ///< Initial regression sum of squares 
};

#endif // FORWARDSELECTION_HH
