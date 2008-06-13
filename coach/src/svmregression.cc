#include "svmregression.hh"

#include <cassert>
#include <torch/QCTrainer.h>
#include <torch/SVMRegression.h>

SVMRegression::SVMRegression(const MSZDataSet &data) 
  : LearningAlgorithm(data), kernelType(DOT) { }

SVMRegression::SVMRegression(SVMKernel kernelType, const MSZDataSet &data)
  : LearningAlgorithm(data), kernelType(kernelType) { }

SVMRegression::~SVMRegression() { }

Model SVMRegression::run() {
  
  // Set the dataset for SVM
  Torch::Kernel *k = 0;

  // Create SVM
  switch(kernelType) {
  case DOT:
    k = new Torch::DotKernel((real)(params[0]));
    break;
  case POLYNOMIAL:
    k = new Torch::PolynomialKernel((int)(params[0]), (real)(params[1]), (real)(params[2]));
    break;
  case GAUSSIAN:
    k = new Torch::GaussianKernel((real)(params[0]));
    break;
  default:
    assert(false);
    break;
  }
  
  // Create Trainer
  Torch::SVMRegression alg(k);
  Torch::QCTrainer trainer(&alg);

  // Transform out dataset to their dataset

  // Generate Model
  trainer.train(ds, NULL);

  // Get results into our model and return

}
