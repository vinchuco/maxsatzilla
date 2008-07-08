#ifndef SVMMODEL_HH
#define SVMMODEL_HH

#include "model.hh"
#include <svm.h>

#include <map>

using std::map;

class SVMModel : public Model {
public:
  SVMModel();
  SVMModel(const char *);
  SVMModel(struct svm_problem *, struct svm_model *);
  virtual ~SVMModel();

  void setStructs(struct svm_problem *, struct svm_model *);
  void setStructsFromFile(const char *);
  struct svm_model *getSVMModelStruct() const { return model; }
  void setFeatureLabel(int, string); ///< Sets label for feature i, 0-based.
  double computeModelOutput(const map<string, double>&) const;

  // Not implementable
  void addRegressor(double, const FeatureLabel&);
  void addRegressor(double);

private:
  struct svm_problem *prob;
  struct svm_model *model;
  map<string, int> idxs; ///< Labels for features indexed by an int
};

#endif // SVMMODEL_HH
