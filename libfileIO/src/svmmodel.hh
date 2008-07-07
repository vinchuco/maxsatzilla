#ifndef SVMMODEL_HH
#define SVMMODEL_HH

#include "model.hh"
#include <svm.h>

class SVMModel : public Model {
public:
  SVMModel();
  SVMModel(const char *);
  SVMModel(struct svm_problem *, struct svm_model *);
  ~SVMModel();

  void setStructs(struct svm_problem *, struct svm_model *);
  void setStructsFromFile(const char *);

  // Not implementable
  void addRegressor(double, const FeatureLabel&);
  void addRegressor(double);

private:
  struct svm_problem *prob;
  struct svm_model *model;
};

#endif // SVMMODEL_HH
