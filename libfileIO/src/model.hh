#ifndef MODEL_HH
#define MODEL_HH

class Model {
public:
  Model();
  virtual ~Model();

  virtual void addRegressor(double, const FeatureLabel&) = 0;
  virtual void addRegressor(double) = 0;
 
private: 

};

#endif // MODEL_HH
