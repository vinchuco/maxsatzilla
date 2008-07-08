#ifndef MODEL_HH
#define MODEL_HH

#include "featurelabel.hh"
#include "learningalg.hh"

class Model {
public:
  Model(LearningAlg);
  virtual ~Model();

  virtual void addRegressor(double, const FeatureLabel&) = 0;
  virtual void addRegressor(double) = 0;
 
  virtual LearningAlg getLearningAlg() const { return la; }

protected:
  LearningAlg la; ///< Learning algorithm used to learn the model
};

#endif // MODEL_HH
