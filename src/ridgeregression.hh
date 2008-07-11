#ifndef RIDGEREGRESSION_HH
#define RIDGEREGRESSION_HH

#include <string>
#include <vector>

#include <gsl/gsl_matrix.h>

#include "dataset.hh"
#include "rrmodel.hh"
#include "learningalgorithm.hh" 

using std::string;
using std::vector;

/** This class implements
 *  the ridge regression algorithm.
 *
 *  I'm still pondering if there's any logic at all of this being a class.
 *  I don't think there is but I'll leave it like this for now!
 */
class RidgeRegression : public LearningAlgorithm {
public:
  RidgeRegression(const DataSet &);
  virtual ~RidgeRegression();

  void setDelta(double d) { delta = d; }
  
  // Runs ridge regression algorithm and outputs 
  // header file with static definition of model
  // for a given output.
  virtual RRModel * run();

private:
  double delta;           ///< Delta value for ridge regression
};

#endif // RIDGEREGRESSION_HH
