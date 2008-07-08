#ifndef MODEL_HH
#define MODEL_HH

#include "featurelabel.hh"
#include "learningalgtype.hh"

class Model {
public:
  Model(LearningAlgType);
  virtual ~Model();

  virtual void addRegressor(double, const FeatureLabel&) = 0;
  virtual void addRegressor(double) = 0;
 
  virtual LearningAlgType getLearningAlgType() const { return la; }

protected:
  LearningAlgType la; ///< Learning algorithm used to learn the model
};

#endif // MODEL_HH
