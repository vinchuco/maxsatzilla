#ifndef SVMREGRESSION_HH
#define SVMREGRESSION_HH

#include "dataset.hh"
#include "learningalgorithm.hh"
#include "svmmodel.hh"

class SVMRegression : public LearningAlgorithm {
public:
  enum RegressionType {NU_R = NU_SVR, EPSILON_R = EPSILON_SVR};
  enum KernelType {LINEAR_K = LINEAR, POLY_K = POLY, RBF_K = RBF, SIGMOID_K = SIGMOID};

  SVMRegression(const SVMParams &, const DataSet &);
  virtual ~SVMRegression();

  // Runs an SVM regression algorithm and outputs
  // header file with static definition of model
  // for a given output
  virtual SVMModel* run();

private:
  void checkParams(); 
  string regressionTypeName(RegressionType) const;
  string kernelTypeName(KernelType) const;

  RegressionType regressionType; ///< Type of regression
  KernelType kernelType;          ///< Kernel to be used on regression

  // The ...Default parameters are used to warn the user if the default
  // values will be used instead of the user-set ones.
  
  uint degree;
  bool degreeDefault;

  double gamma;
  bool gammaDefault;

  double coef0;
  bool coef0Default;

  double cacheSize;
  bool cacheSizeDefault;

  double eps;
  bool epsDefault;

  double cost;
  bool costDefault;

  double nu;
  bool nuDefault;

  double p;
  bool pDefault;

  bool shrinking;
  bool shrinkingDefault;

  bool probability;
  bool probabilityDefault;
};

#endif // SVMREGRESSION_HH
