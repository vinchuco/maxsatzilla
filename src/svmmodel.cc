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

/// Sets up feature name for a given feature index.
void SVMModel::setFeatureLabel(int index, string name) {

  const map<string, int>::const_iterator it = idxs.find(name);
  if(it == idxs.end())
    cerr << "SVMModel: setting feature label previously set. overwriting!";

  idxs[name] = index;
}

double SVMModel::computeModelOutput(const map<string, double>& model) const {

  struct svm_node *nodes = new struct svm_node [model.size()+1];
  const map<string,double>::const_iterator& modelEnd = model.end();

  nodes[model.size()].index = -1;

  uint i = 0;  
  for(map<string, double>::const_iterator it = model.begin();
      it != modelEnd;
      ++it, ++i) { 
    
    const map<string, int>::const_iterator idxit = idxs.find(it->first);
    if(idxit == idxs.end()) {
      cerr << "SVMModel::computeModelOutput : Couldn't find index for feature " << it->first << "\n";
      exit(EXIT_FAILURE);
    }

    nodes[i].index = idxs[it->first];
    nodes[i].value = it->second;
  }
  delete[] nodes;

}
