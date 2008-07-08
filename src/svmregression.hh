#ifndef SVMREGRESSION_HH
#define SVMREGRESSION_HH

#include "dataset.hh"
#include "learningalgorithm.hh"
#include "svmmodel.hh"

class SVMRegression : public LearningAlgorithm {
public:
  enum RegressionType {NU_R, EPSILON_R};
  enum SVMKernel {LINEAR_K, POLY_K, RBF_K, SIGMOID_K};

  SVMRegression(const MSZDataSet &);
  SVMRegression(SVMKernel, const MSZDataSet &);
  virtual ~SVMRegression();
  
  // Runs an SVM regression algorithm and outputs
  // header file with static definition of model
  // for a given output
  virtual SVMModel* run();

private:
  RegressionType regressionType; ///< Type of regression
  SVMKernel kernelType;          ///< Kernel to be used on regression
  
  int degree;
  double gamma;
  double coef0;
  
  double cacheSize;
  double eps;
  double C;
  double nu;
  double p;
  int shrinking;
  int probability;
};

#endif // SVMREGRESSION_HH
