#ifndef SVMREGRESSION_HH
#define SVMREGRESSION_HH

#include "dataset.hh"
#include "learningalgorithm.hh"
#include "model.hh"
#include <torch/general.h>

class SVMRegression : public LearningAlgorithm {
public:
  enum SVMKernel {DOT, POLYNOMIAL, GAUSSIAN};

  SVMRegression(const MSZDataSet &);
  SVMRegression(SVMKernel, const MSZDataSet &);
  ~SVMRegression();
  
  void setParams(const real p[]) { for(int i = 0; i < 3; ++i) params[i] = p[i]; }

  // Runs an SVM regression algorithm and outputs
  // header file with static definition of model
  // for a given output
  virtual Model run();

private:
  SVMKernel kernelType;   ///< Kernel to be used on regression
  real params[3];         ///< Parameters
};

#endif // SVMREGRESSION_HH
