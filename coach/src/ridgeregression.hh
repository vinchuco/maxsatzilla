#ifndef RIDGEREGRESSION_HH
#define RIDGEREGRESSION_HH

#include <string>
#include <vector>

#include <gsl/gsl_matrix.h>

#include "dataset.hh"
#include "model.hh"

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
  Model run(double);

private:
  const MSZDataSet &data; ///< Reference to data is kept.
};

#endif // RIDGEREGRESSION_HH