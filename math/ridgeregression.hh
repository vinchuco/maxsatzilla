#ifndef RIDGEREGRESSION_HH
#define RIDGEREGRESSION_HH

#include <string>
#include <vector>

#include<gsl/gsl_matrix.h>

#include "dataset.hh"

#define RRDEBUG 1

using std::string;
using std::vector;

/** This class implements
 *  the ridge regression algorithm.
 *
 *  I'm still pondering if there's any logic at all of this being a class.
 *  I don't think there is but I'll leave it like this for now!
 */
class RidgeRegression {
public:
  RidgeRegression(const MSZDataSet &);
  ~RidgeRegression();
  
  // Runs ridge regression algorithm and outputs 
  // header file with static definition of model
  // for a given output.
  vector<double> run(double, size_t);

private:
  gsl_matrix *matrix_mult(const gsl_matrix *, const gsl_matrix *) const;

  const MSZDataSet &data; ///< Reference to data is kept.
};

#endif // RIDGEREGRESSION_HH
