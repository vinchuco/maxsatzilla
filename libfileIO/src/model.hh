#ifndef MODEL_HH
#define MODEL_HH

class Model {
public:
  Model();
  virtual ~Model();

  virtual void addRegressor(double, const FeatureLabel&) = 0;
  virtual void addRegressor(double) = 0;
  virtual void remRegressor(const FeatureLabel&) = 0;
  virtual double getRegressor(const FeatureLabel&) const = 0;

private:

};

#endif // MODEL_HH
