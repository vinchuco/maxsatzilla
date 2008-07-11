#ifndef SVMREGRESSION_HH
#define SVMREGRESSION_HH

#include "dataset.hh"
#include "learningalgorithm.hh"
#include "svmmodel.hh"

class SVMRegression : public LearningAlgorithm {
public:
  enum RegressionType {NU_R, EPSILON_R};
  enum KernelType {LINEAR_K, POLY_K, RBF_K, SIGMOID_K};

  SVMRegression(const MSZDataSet &);
  SVMRegression(RegressionType, KernelType, const MSZDataSet &);
  virtual ~SVMRegression();

  void setRegressionType(RegressionType rt) { regressionType = rt; }
  void setKernelType(KernelType kt) { kernelType = kt; }
  
  void setDegree(uint d)  { degree = d;      }
  void setGamma(double g) { gamma = g;       }
  void setCoef0(double c0){ cacheSize = c0;  } 
  void setCacheSize(double cs){ cacheSize = cs; }
  void setStopCrit(double sc) { eps = sc;    }
  void setC(double c_)    { c = c_;          }
  void setNu(double nu_)  { nu = nu_;        }
  void setP(double p_)    { p = p_;          }
  void setShrinking()     { shrinking = true;}
  void clearShrinking()   { shrinking = false;}
  void setProbability()   { probability = true; }
  void clearProbability() { probability = false; }

  // Runs an SVM regression algorithm and outputs
  // header file with static definition of model
  // for a given output
  virtual SVMModel* run();

private:
  RegressionType regressionType; ///< Type of regression
  KernelType kernelType;          ///< Kernel to be used on regression
  
  uint degree;
  double gamma;
  double coef0;
  
  double cacheSize;
  double eps;
  double c;
  double nu;
  double p;
  bool shrinking;
  bool probability;
};

#endif // SVMREGRESSION_HH
