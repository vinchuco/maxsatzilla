#ifndef SVMMODEL_HH
#define SVMMODEL_HH

#include "model.hh"
#include <svm.h>

class SVMModel : public Model {
public:
  SVMModel(struct svm_problem *, struct svm_model *);
  ~SVMModel();

private:
  struct svm_problem *prob;
  struct svm_model *model;
};

#endif // SVMMODEL_HH
