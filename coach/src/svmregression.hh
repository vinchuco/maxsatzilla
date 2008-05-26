#ifndef SVMREGRESSION_HH
#define SVMREGRESSION_HH

enum SVMKernel {DOT, POLYNOMIAL, GAUSSIAN};

class SVMRegression {
public:
  SVMRegression(const MSZDataSet &);
  SVMRegression(SVMKernel, const MSZDataSet &);
  ~SVMRegression();

  // Runs an SVM regression algorithm and outputs
  // header file with static definition of model
  // for a given output
  Model run(const real *params);

private:
  const MSZDataSet &data; ///< Reference to data is kept
  SVMKernel kernelType; ///< Kernel to be used on regression
};

#endif // SVMREGRESSION_HH
