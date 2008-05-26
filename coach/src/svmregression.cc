#include "svmregression.hh"

SVMRegression::SVMRegression(const MSZDataSet &data) 
  : data(data), kernelType(DOT) {


}

SVMRegression::SVMRegression(SVMKernel kernelType, const MSZDataSet &data)
  : data(data), kernelType(kernelType) {


}

SVMRegression::~SVMRegression() {



}

Model run(const real *params) {
  
  // Set the dataset for SVM
  Kernel *k = 0;

  // Create SVM
  switch(kernel) {
  case DOT:
    k = new DotKernel((real)(params[0]));
    break;
  case POLYNOMIAL:
    k = new PolynomialKernel((int)(params[0]), (real)(params[1]), (real)(params[2]));
    break;
  case GAUSSIAN:
    k = new GaussianKernel((real)(params[0]));
    break;
  default:
    assert(false);
    break;
  }
  
  // Create Trainer
  SVMRegression alg(k);
  QCTrainer trainer(&alg);

  // Generate Model
  

}
