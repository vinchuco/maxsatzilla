#include "svmregression.hh"

#include "logmgm.hh"

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <svm.h>

SVMRegression::SVMRegression(const DataSet &data) 
  : LearningAlgorithm(data), regressionType(NU_R), kernelType(RBF_K),
    degree(3), degreeDefault(true),
    gamma(1.0/data.getNFeatures()), gammaDefault(true),
    coef0(0.0), coef0Default(true),
    cacheSize(100.0), cacheSizeDefault(true),
    eps(0.001), epsDefault(true),
    cost(1.0), costDefault(true),
    nu(0.5), nuDefault(true),
    p(0.1), pDefault(true),
    shrinking(true), shrinkingDefault(true),
    probability(false), probabilityDefault(true)
{ }

SVMRegression::SVMRegression(RegressionType regressionType, KernelType kernelType, const DataSet &data)
  : LearningAlgorithm(data), regressionType(regressionType), kernelType(kernelType),
    degree(3), degreeDefault(true),
    gamma(1.0/data.getNFeatures()), gammaDefault(true),
    coef0(0.0), coef0Default(true),
    cacheSize(100.0), cacheSizeDefault(true),
    eps(0.001), epsDefault(true),
    cost(1.0), costDefault(true),
    nu(0.5), nuDefault(true),
    p(0.1), pDefault(true),
    shrinking(true), shrinkingDefault(true),
    probability(false), probabilityDefault(true)
{ }

SVMRegression::~SVMRegression() { }

SVMModel *SVMRegression::run() {
  
  checkParams();

  // Create SVM
  struct svm_parameter *svm_params = new struct svm_parameter;
  svm_params->svm_type = regressionType;
  svm_params->kernel_type = kernelType;
  
  svm_params->degree = degree;
  svm_params->gamma = gamma;
  svm_params->coef0 = coef0;
  svm_params->cache_size = cacheSize;
  svm_params->eps = eps;
  svm_params->C = cost;
  svm_params->nu = nu;
  svm_params->p = p;
  svm_params->shrinking = shrinking ? 1 : 0;
  svm_params->probability = probability ? 1 : 0;

  // Create an SVM problem
  svm_problem *svmp = new svm_problem;
  svmp->l = data.getNRows();

  double *y = new double [data.getNRows()];
  for(unsigned int i = 0; i < data.getNRows(); ++i)
    y[i] = data.getOutputValue(i);
  svmp->y = y;

  struct svm_node **x = new struct svm_node* [data.getNRows()];
  for(unsigned int i = 0; i < data.getNRows(); ++i) {
    x[i] = new struct svm_node [data.getNFeatures() + 1];
    
    for(unsigned int f = 0; f <= data.getNFeatures(); ++f) {
      if(f != data.getNFeatures()) {
	x[i][f].index = f+1;
	x[i][f].value = data.getFeatureValue(i, f);
      }
      else
	x[i][f].index = -1;
    }
  }
  svmp->x = x;
    
  // Generate Model
  
  // Check validity of parameters
  const char *err = svm_check_parameter(svmp, svm_params);
  if(err != NULL) {
    std::cerr << "SVMParams error: " << err << "\n";
    exit(EXIT_FAILURE);
  }
  
  struct svm_model *model = svm_train(svmp, svm_params);

  // Get results into our model and return
  SVMModel * m = new SVMModel(svmp, model);

  return m;
}

string SVMRegression::regressionTypeName(RegressionType type) const {
  string name;
  switch(type) {
  case NU_R:
    name = "Nu";
    break;
  case EPSILON_R:
    name = "Epsilon";
    break;
  default:
    name = "Unknown";
    break;
  }
  return name;
}

string SVMRegression::kernelTypeName(KernelType type) const {
  string name;
  switch(type) {
  case LINEAR_K:
    name = "Linear";
    break;
  case POLY_K:
    name = "Polynomial";
    break;
  case RBF_K:
    name = "Radial Basis Function";
    break;
  case SIGMOID_K:
    name = "Sigmoid";
    break;
  default:
    name = "Unknown";
    break;
  }
  return name;
}

void SVMRegression::checkParams() {

  *(LogMgm::Instance()) << "SVM Regression " << regressionTypeName(regressionType)
			<< " with a " << kernelTypeName(kernelType) << " kernel\n";

  if(regressionType == EPSILON_R || 
     regressionType == NU_R)
    if(costDefault)
      *(LogMgm::Instance()) << "Warning: Using cost default value " << cost << "\n";
  
  if(regressionType == EPSILON_R)
    if(pDefault) 
      *(LogMgm::Instance()) << "Warning: Using epsilon default value " << p << "\n";
     
  if(regressionType == NU_R)
    if(nuDefault)
      *(LogMgm::Instance()) << "Warning: Using nu default value " << nu << "\n";

  if(kernelType == POLY_K)
    if(degreeDefault)
      *(LogMgm::Instance()) << "Warning: Using degree default value " << degree << "\n";

  if(kernelType == POLY_K || 
     kernelType == RBF_K ||
     kernelType == SIGMOID_K)
    if(gammaDefault)
      *(LogMgm::Instance()) << "Warning: Using gamma default value " << gamma << "\n";
  
  if(kernelType == POLY_K ||
     kernelType == SIGMOID_K)
    if(coef0Default)
      *(LogMgm::Instance()) << "Warning: Using coef0 default value " << coef0 << "\n";

  if(shrinkingDefault)
    *(LogMgm::Instance()) << "Warning: Using shrinking default value " << shrinking << "\n";

  if(probabilityDefault)
    *(LogMgm::Instance()) << "Warning: Using probability default value " << probability << "\n";

  if(cacheSizeDefault)
    *(LogMgm::Instance()) << "Warning: Using cacheSize default value " << cacheSize << "\n";

  if(epsDefault) 
    *(LogMgm::Instance()) << "Warning: Using epsilon default value " << eps << "\n";

}
