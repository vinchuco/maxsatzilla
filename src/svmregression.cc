#include "svmregression.hh"

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <svm.h>

SVMRegression::SVMRegression(const MSZDataSet &data) 
  : LearningAlgorithm(data), regressionType(NU_R), kernelType(RBF_K) { }

SVMRegression::SVMRegression(RegressionType regressionType, KernelType kernelType, const MSZDataSet &data)
  : LearningAlgorithm(data), regressionType(regressionType), kernelType(kernelType) { }

SVMRegression::~SVMRegression() { }

SVMModel *SVMRegression::run() {
  
  // Create SVM
  struct svm_parameter *svm_params = new struct svm_parameter;
  svm_params->svm_type = regressionType;
  svm_params->kernel_type = kernelType;
  
  svm_params->degree = degree;
  svm_params->gamma = gamma;
  svm_params->coef0 = coef0;
  svm_params->cache_size = cacheSize;
  svm_params->eps = eps;
  svm_params->C = c;
  svm_params->nu = nu;
  svm_params->p = p;
  svm_params->shrinking = shrinking;
  svm_params->probability = probability;

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

