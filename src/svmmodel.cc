#include "svmmodel.hh"

#include <iostream>

#include <cstdlib>

using std::cerr;

SVMModel::SVMModel()
  : Model(SVM), prob(NULL), model(NULL) { }

SVMModel::SVMModel(struct svm_problem *prob, struct svm_model *model) 
  : Model(SVM), prob(prob), model(model) { }

SVMModel::SVMModel(const char *fname) 
  : Model(SVM), prob(NULL) {
  setStructsFromFile(fname);
}

SVMModel::~SVMModel() {
  svm_destroy_model(model);
  free(prob);
}

void SVMModel::setStructs(struct svm_problem *svmp, struct svm_model *svmm) {
  prob = svmp;
  model = svmm;
}

void SVMModel::setStructsFromFile(const char *fname) {
  model = svm_load_model(fname);
  if(model == NULL)
    cerr << "SVMModel : Unable to load model from " << fname << "\n";
}

void SVMModel::addRegressor(double, const FeatureLabel&) {
  cerr << "SVMModel : Should not have been called.";
  exit(EXIT_FAILURE);
}

void SVMModel::addRegressor(double) {
  cerr << "SVMModel : Should not have been called.";
  exit(EXIT_FAILURE);
}
