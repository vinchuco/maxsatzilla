#include "svmmodel.hh"

SVMModel::SVMModel(struct svm_problem *prob, struct svm_model *model) 
  : prob(prob), model(model) { }

SVMModel::~SVMModel() {
  svm_destroy_model(model);
  free(prob)
}
